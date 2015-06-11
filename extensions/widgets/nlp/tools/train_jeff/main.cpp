#include <mitie/ner_trainer.h>
#include <iostream>

#include <tinyxml2.h>

using namespace dlib;
using namespace std;
using namespace mitie;

typedef std::vector<std::string> qtz_string_list;

typedef struct {
  unsigned long index;
  unsigned long token_count;
  const char *name;
} qtz_entity_type;

typedef struct {
  std::vector<std::string> sentence;
  std::vector<qtz_entity_type> entity_list;
} qtz_training_data;

typedef std::vector<qtz_entity_type> qtz_entity_list;

qtz_string_list
qtz_build_word_list_from_string(const std::string &a_str) {
  std::stringstream data_stream(a_str);
  std::istream_iterator<std::string> begin(data_stream);
  std::istream_iterator<std::string> end;
  qtz_string_list rv(begin, end);

  return rv;
}

ner_training_instance
qtz_build_sentance(const qtz_string_list &a_sentence_vlist,
                   const qtz_entity_list &a_entity_vlist) {
  ner_training_instance rv(a_sentence_vlist);

  std::for_each(std::begin(a_entity_vlist),
                std::end(a_entity_vlist), [&](qtz_entity_type a_entity){
    rv.add_entity(a_entity.index, a_entity.token_count, a_entity.name);
  });

  return rv;
}

std::vector<ner_training_instance> qtz_build_data_set() {
  std::vector<ner_training_instance> sentence_list;

  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError error = doc.LoadFile("train.xml");
  if (error != tinyxml2::XML_NO_ERROR) {
      std::cout << " Error " << doc.GetErrorStr1() << std::endl;
    return sentence_list;
  }

  tinyxml2::XMLElement *root = doc.FirstChildElement("body");

  for(tinyxml2::XMLNode *child = root->FirstChild(); child != NULL;
      child = child->NextSibling()) {
      std::cout << "-" << child->ToElement()->Name() << std::endl;

      qtz_string_list str_vlist;

      for(tinyxml2::XMLElement *sentence = child->FirstChildElement("text");
          sentence != NULL;
          sentence = sentence->NextSiblingElement("text")) {
          std::cout << "-" << sentence->GetText() << std::endl;
          str_vlist = qtz_build_word_list_from_string(sentence->GetText());
      }

     qtz_entity_list entity_list;
     for(tinyxml2::XMLElement *tag = child->FirstChildElement("tag");
          tag != NULL;
          tag = tag->NextSiblingElement("tag")) {

          qtz_entity_type entity;
          if (tag->Attribute("index")) {
               entity.index = atoi(tag->Attribute("index"));
          }

          if (tag->Attribute("tokens")) {
               entity.token_count = atoi(tag->Attribute("tokens"));
          }

          entity.name = tag->GetText();

          entity_list.push_back(entity);
      }

     sentence_list.push_back(qtz_build_sentance(str_vlist, entity_list));
  }

  return sentence_list;
}

int main(int argc, char** argv)
{
  std::vector<ner_training_instance> sentence_list = qtz_build_data_set();

  if (argc != 2)
    {
      cout << "You must give the path to the MITIE English total_word_feature_extractor.dat file." << endl;
      cout << "So run this program with a command like: " << endl;
      cout << "./train_ner_example ../../../MITIE-models/english/total_word_feature_extractor.dat" << endl;
      return 1;
    }

  ner_trainer trainer(argv[1]);

  std::for_each(std::begin(sentence_list),
                std::end(sentence_list), [&] (ner_training_instance a_sentence) {

    trainer.add(a_sentence);
  });

  trainer.set_num_threads(4);
  named_entity_extractor ner = trainer.train();

  serialize("jeff_ner_model.dat") << "mitie::named_entity_extractor" << ner;

/*
  const std::vector<string> tagstr = ner.get_tag_name_strings();
  cout << "The tagger supports "<< tagstr.size() <<" tags:" << endl;
  for (unsigned int i = 0; i < tagstr.size(); ++i)
    cout << "   " << tagstr[i] << endl;

  std::vector<std::string> sentence3 =
      qtz_build_word_list_from_string("is it sunny today ?");

  std::vector<pair<unsigned long, unsigned long> > chunks;
  std::vector<unsigned long> chunk_tags;
  ner(sentence3, chunks, chunk_tags);

  cout << "\nNumber of named entities detected: " << chunks.size() << endl;
  for (unsigned int i = 0; i < chunks.size(); ++i)
    {
      cout << "   Tag " << chunk_tags[i] << ":" << tagstr[chunk_tags[i]] << ": ";
      // chunks[i] defines a half open range in sentence3 that contains the entity.
      for (unsigned long j = chunks[i].first; j < chunks[i].second; ++j)
        cout << sentence3[j] << " ";
      cout << endl;
    }
*/
}

