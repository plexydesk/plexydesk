#include "events.hpp"
#include <cstring>
#include <cstdio>

namespace plexy::ui {


extern const char* string_table_get(StringId id);
extern void string_table_update(StringId id, const char* text);

static int count_nonempty_lines(const char* text) {
    if (!text || !text[0]) return 0;
    int count = 0;
    const char* line_start = text;
    const char* p = text;
    while (1) {
        if (*p == '\n' || *p == '\0') {
            if (p > line_start) count++;
            if (*p == '\0') break;
            line_start = p + 1;
        }
        p++;
    }
    return count;
}



void EventQueue::push(const UIEvent& event) {
    
    if (event.type == UIEventType::MouseMove && has_pending_move_) {
        buffer_[pending_move_idx_] = event;
        return;
    }

    if (count_ >= QUEUE_SIZE) return; 

    uint32_t idx = tail_;
    buffer_[tail_] = event;
    tail_ = (tail_ + 1) & (QUEUE_SIZE - 1);
    count_++;

    if (event.type == UIEventType::MouseMove) {
        has_pending_move_ = true;
        pending_move_idx_ = idx;
    }
}

bool EventQueue::pop(UIEvent* out) {
    if (count_ == 0) return false;
    *out = buffer_[head_];

    if (buffer_[head_].type == UIEventType::MouseMove) {
        has_pending_move_ = false;
    }

    head_ = (head_ + 1) & (QUEUE_SIZE - 1);
    count_--;
    return true;
}

bool EventQueue::empty() const { return count_ == 0; }

void EventQueue::clear() {
    head_ = tail_ = count_ = 0;
    has_pending_move_ = false;
}



void SpatialGrid::init(int screen_w, int screen_h) {
    screen_w_ = screen_w;
    screen_h_ = screen_h;
    cols_ = (screen_w + CELL_SIZE - 1) / CELL_SIZE;
    rows_ = (screen_h + CELL_SIZE - 1) / CELL_SIZE;

    int total = cols_ * rows_;
    cells_ = new Cell[total];
    std::memset(cells_, 0, sizeof(Cell) * total);
}

void SpatialGrid::shutdown() {
    delete[] cells_;
    cells_ = nullptr;
}

SpatialGrid::Cell* SpatialGrid::cell_at(int col, int row) {
    if (col < 0 || col >= cols_ || row < 0 || row >= rows_) return nullptr;
    return &cells_[row * cols_ + col];
}

void SpatialGrid::rebuild(Widget* root) {
    
    int total = cols_ * rows_;
    for (int i = 0; i < total; i++) {
        cells_[i].count = 0;
    }

    
    if (root) insert_widget(root);
}

void SpatialGrid::insert_widget(Widget* w) {
    if (!w->visible()) return;

    float x = 0.0f, y = 0.0f, ww = 0.0f, hh = 0.0f;
    widget_effective_rect(w, &x, &y, &ww, &hh);
    float r = x + ww, b = y + hh;

    
    bool interactive = w->enabled() && (
        w->type == WidgetType::Button ||
        w->type == WidgetType::IconButton ||
        w->type == WidgetType::ToggleButton ||
        w->type == WidgetType::TextInput ||
        w->type == WidgetType::TextArea ||
        w->type == WidgetType::Checkbox ||
        w->type == WidgetType::RadioButton ||
        w->type == WidgetType::Switch ||
        w->type == WidgetType::Slider ||
        w->type == WidgetType::List ||
        w->type == WidgetType::IconView ||
        w->type == WidgetType::Dropdown ||
        w->type == WidgetType::ScrollView ||
        w->type == WidgetType::Panel
    );

    if (interactive && ww > 0 && hh > 0) {
        int col0 = (int)(x / CELL_SIZE);
        int row0 = (int)(y / CELL_SIZE);
        int col1 = (int)(r / CELL_SIZE);
        int row1 = (int)(b / CELL_SIZE);

        for (int row = row0; row <= row1; row++) {
            for (int col = col0; col <= col1; col++) {
                Cell* cell = cell_at(col, row);
                if (cell && cell->count < MAX_PER_CELL) {
                    cell->widgets[cell->count++] = w;
                }
            }
        }
    }

    
    for (Widget* c = w->first_child; c; c = c->next_sibling) {
        insert_widget(c);
    }
}

Widget* SpatialGrid::hit_test(float x, float y) {
    int col = (int)(x / CELL_SIZE);
    int row = (int)(y / CELL_SIZE);
    Cell* cell = cell_at(col, row);
    if (!cell) return nullptr;

    
    Widget* best = nullptr;
    for (int i = (int)cell->count - 1; i >= 0; i--) {
        Widget* w = cell->widgets[i];
        float wx = 0.0f, wy = 0.0f, ww = 0.0f, wh = 0.0f;
        widget_effective_rect(w, &wx, &wy, &ww, &wh);
        if (x >= wx && x < wx + ww &&
            y >= wy && y < wy + wh) {
            best = w;
            break; 
        }
    }
    return best;
}



void FocusManager::set_focus(Widget* w) {
    if (focused_ == w) return;
    if (focused_) focused_->clear_flag(WIDGET_FOCUSED);
    focused_ = w;
    if (focused_) focused_->set_flag(WIDGET_FOCUSED);
}

void FocusManager::clear_focus() {
    if (focused_) focused_->clear_flag(WIDGET_FOCUSED);
    focused_ = nullptr;
}

void FocusManager::collect_focusable(Widget* root) {
    focusable_count_ = 0;

    
    struct StackEntry { Widget* w; };
    StackEntry stack[512];
    int sp = 0;
    stack[sp++] = {root};

    while (sp > 0 && focusable_count_ < MAX_FOCUSABLE) {
        Widget* w = stack[--sp].w;
        if (!w->visible() || !w->enabled()) continue;

        bool focusable = (
            w->type == WidgetType::Button ||
            w->type == WidgetType::TextInput ||
            w->type == WidgetType::TextArea ||
            w->type == WidgetType::Checkbox ||
            w->type == WidgetType::RadioButton ||
            w->type == WidgetType::Switch ||
            w->type == WidgetType::Slider ||
            w->type == WidgetType::List ||
            w->type == WidgetType::IconView ||
            w->type == WidgetType::Dropdown
        );

        if (focusable) {
            focusable_[focusable_count_++] = w;
        }

        
        Widget* last = nullptr;
        for (Widget* c = w->first_child; c; c = c->next_sibling) last = c;
        for (Widget* c = last; c; ) {
            Widget* prev = nullptr;
            for (Widget* s = w->first_child; s && s->next_sibling != c; s = s->next_sibling) prev = s->next_sibling ? s : nullptr;
            
            if (sp < 512) stack[sp++] = {c};
            
            break;
        }
        
        for (Widget* c = w->first_child; c; c = c->next_sibling) {
            if (sp < 512) stack[sp++] = {c};
        }
    }
}

int FocusManager::find_index(Widget* w) {
    for (uint32_t i = 0; i < focusable_count_; i++) {
        if (focusable_[i] == w) return (int)i;
    }
    return -1;
}

void FocusManager::focus_next(Widget* root) {
    collect_focusable(root);
    if (focusable_count_ == 0) return;

    int idx = find_index(focused_);
    int next = (idx + 1) % (int)focusable_count_;
    set_focus(focusable_[next]);
}

void FocusManager::focus_prev(Widget* root) {
    collect_focusable(root);
    if (focusable_count_ == 0) return;

    int idx = find_index(focused_);
    int prev = (idx <= 0) ? (int)focusable_count_ - 1 : idx - 1;
    set_focus(focusable_[prev]);
}



void EventProcessor::set_hovered(Widget* w) {
    if (hovered_ == w) return;
    if (hovered_) {
        hovered_->clear_flag(WIDGET_HOVERED);
        hovered_->mark_dirty();
    }
    hovered_ = w;
    if (hovered_) {
        hovered_->set_flag(WIDGET_HOVERED);
        hovered_->mark_dirty();
    }
}

bool EventProcessor::process_all(EventQueue* queue, SpatialGrid* grid,
                                  FocusManager* focus, Widget* root) {
    UIEvent ev;
    bool changed = false;
    while (queue->pop(&ev)) {
        switch (ev.type) {
            case UIEventType::MouseMove:
                if (handle_mouse_move(ev, grid, focus)) changed = true;
                break;
            case UIEventType::MouseButton:
                handle_mouse_button(ev, grid, focus);
                changed = true; 
                break;
            case UIEventType::MouseScroll:
                handle_mouse_scroll(ev, grid);
                changed = true;
                break;
            case UIEventType::KeyDown:
            case UIEventType::KeyUp:
                handle_key(ev, focus);
                changed = true;
                break;
            case UIEventType::CharInput:
                handle_char_input(ev, focus);
                changed = true;
                break;
        }
    }
    return changed;
}

bool EventProcessor::handle_mouse_move(const UIEvent& ev, SpatialGrid* grid,
                                        FocusManager* focus) {
    Widget* hit = grid->hit_test(ev.x, ev.y);
    bool changed = (hit != hovered_);
    set_hovered(hit);

    
    if (pressed_ && pressed_->type == WidgetType::Slider) {
        float slider_x = 0.0f, slider_w = 0.0f;
        widget_effective_rect(pressed_, &slider_x, nullptr, &slider_w, nullptr);
        float t = (ev.x - slider_x) / slider_w;
        if (t < 0) t = 0;
        if (t > 1) t = 1;
        pressed_->value = pressed_->value_min + t * (pressed_->value_max - pressed_->value_min);
        pressed_->mark_dirty();
        if (pressed_->on_value_change) {
            pressed_->on_value_change(pressed_->id, pressed_->value, pressed_->on_value_change_data);
        }
        changed = true;
    }
    return changed;
}

void EventProcessor::handle_mouse_button(const UIEvent& ev, SpatialGrid* grid,
                                          FocusManager* focus) {
    Widget* hit = grid->hit_test(ev.x, ev.y);

    if (ev.pressed && ev.button == 0) {
        
        pressed_ = hit;
        press_x_ = ev.x;
        press_y_ = ev.y;

        if (hit) {
            hit->set_flag(WIDGET_PRESSED);
            hit->mark_dirty();
            focus->set_focus(hit);
        } else {
            focus->clear_focus();
        }
    } else if (!ev.pressed && ev.button == 0) {
        
        if (pressed_) {
            pressed_->clear_flag(WIDGET_PRESSED);
            pressed_->mark_dirty();

            
            if (hit == pressed_) {
                switch (pressed_->type) {
                    case WidgetType::Button:
                    case WidgetType::IconButton:
                        if (pressed_->on_click) {
                            pressed_->on_click(pressed_->id, pressed_->on_click_data);
                        }
                        break;

                    case WidgetType::ToggleButton:
                        pressed_->toggle_flag(WIDGET_CHECKED);
                        if (pressed_->on_click) {
                            pressed_->on_click(pressed_->id, pressed_->on_click_data);
                        }
                        break;

                    case WidgetType::Checkbox:
                    case WidgetType::RadioButton:
                        pressed_->toggle_flag(WIDGET_CHECKED);
                        if (pressed_->on_value_change) {
                            pressed_->on_value_change(pressed_->id,
                                pressed_->checked() ? 1.0f : 0.0f,
                                pressed_->on_value_change_data);
                        }
                        break;

                    case WidgetType::Switch:
                        pressed_->toggle_flag(WIDGET_CHECKED);
                        if (pressed_->on_value_change) {
                            pressed_->on_value_change(pressed_->id,
                                pressed_->checked() ? 1.0f : 0.0f,
                                pressed_->on_value_change_data);
                        }
                        break;

                    case WidgetType::List: {
                        float font_size = (pressed_->font_size > 0.0f) ? pressed_->font_size : 14.0f;
                        float line_h = font_size * 1.5f;
                        float pad = 8.0f;
                        float wx = 0.0f, wy = 0.0f;
                        widget_effective_rect(pressed_, &wx, &wy, nullptr, nullptr);
                        int idx = (int)((ev.y - wy - pad) / line_h);
                        int count = count_nonempty_lines(string_table_get(pressed_->text));
                        if (idx >= 0 && idx < count) {
                            pressed_->value = (float)idx;
                            pressed_->mark_dirty();
                            if (pressed_->on_value_change) {
                                pressed_->on_value_change(pressed_->id, pressed_->value,
                                                          pressed_->on_value_change_data);
                            }
                            if (pressed_->on_click) {
                                pressed_->on_click(pressed_->id, pressed_->on_click_data);
                            }
                        }
                        break;
                    }

                    case WidgetType::IconView: {
                        float pad = 18.0f;
                        float gap = 16.0f;
                        float cell_w = 176.0f;
                        float cell_h = 168.0f;
                        float wx = 0.0f, wy = 0.0f, ww = 0.0f;
                        widget_effective_rect(pressed_, &wx, &wy, &ww, nullptr);
                        float local_x = ev.x - wx - pad;
                        float local_y = ev.y - wy - pad + pressed_->scroll_y;
                        int cols = (int)((pressed_->layout.computed_w - pad * 2 + gap) / (cell_w + gap));
                        if (ww > 0.0f) {
                            cols = (int)((ww - pad * 2 + gap) / (cell_w + gap));
                        }
                        if (cols < 1) cols = 1;
                        int col = (int)(local_x / (cell_w + gap));
                        int row = (int)(local_y / (cell_h + gap));
                        if (col >= 0 && col < cols && row >= 0) {
                            float in_cell_x = local_x - col * (cell_w + gap);
                            float in_cell_y = local_y - row * (cell_h + gap);
                            if (in_cell_x >= 0 && in_cell_x <= cell_w &&
                                in_cell_y >= 0 && in_cell_y <= cell_h) {
                                int idx = row * cols + col;
                                int count = count_nonempty_lines(string_table_get(pressed_->text));
                                if (idx >= 0 && idx < count) {
                                    pressed_->value = (float)idx;
                                    pressed_->mark_dirty();
                                    if (pressed_->on_value_change) {
                                        pressed_->on_value_change(pressed_->id, pressed_->value,
                                                                  pressed_->on_value_change_data);
                                    }
                                    if (pressed_->on_click) {
                                        pressed_->on_click(pressed_->id, pressed_->on_click_data);
                                    }
                                }
                            }
                        }
                        break;
                    }

                    default:
                        break;
                }
            }

            pressed_ = nullptr;
        }
    }
}

void EventProcessor::handle_mouse_scroll(const UIEvent& ev, SpatialGrid* grid) {
    Widget* hit = grid->hit_test(ev.x, ev.y);
    if (!hit) return;

    
    Widget* scroll = hit;
    while (scroll && scroll->type != WidgetType::ScrollView &&
           scroll->type != WidgetType::IconView) {
        scroll = scroll->parent;
    }

    if (scroll && scroll->type == WidgetType::IconView) {
        
        float scroll_speed = 40.0f;
        scroll->scroll_y -= ev.scroll_dy * scroll_speed;
        if (scroll->scroll_y < 0) scroll->scroll_y = 0;
        
        scroll->mark_dirty();
        return;
    }

    if (scroll) {
        scroll->value += ev.scroll_dy * 0.02f; 
        if (scroll->value < 0) scroll->value = 0;
        if (scroll->value > 1) scroll->value = 1;
        scroll->mark_dirty();
        if (scroll->on_value_change) {
            scroll->on_value_change(scroll->id, scroll->value, scroll->on_value_change_data);
        }
    }
}

void EventProcessor::handle_key(const UIEvent& ev, FocusManager* focus) {
    
    if (ev.type != UIEventType::KeyDown) return;
    
    
    if (ev.keycode == 9) { 
        if (ev.modifiers & 1) { 
            
        } else {
            
        }
        return;
    }

    Widget* w = focus->focused();
    if (!w) return;

    if (w->on_key) {
        w->on_key(w->id, ev.keycode, ev.modifiers, w->on_key_data);
    }
}

void EventProcessor::handle_char_input(const UIEvent& ev, FocusManager* focus) {
    Widget* w = focus->focused();
    if (!w) return;
    
    
    if (w->type != WidgetType::TextInput && w->type != WidgetType::TextArea) return;

    
    if (w->has_flag(WIDGET_READONLY)) return;

    
    const char* current = string_table_get(w->text);
    if (!current) current = "";

    size_t len = strlen(current);

    
    const size_t MAX_TEXT_SIZE = 1024 * 1024; 
    if (len >= MAX_TEXT_SIZE - 2) return; 

    char* buffer = (char*)malloc(len + 10); 
    if (!buffer) return;
    
    
    if (ev.character == 8 || ev.character == 127) {
        if (len > 0) {
            memcpy(buffer, current, len);
            buffer[len - 1] = '\0';
            string_table_update(w->text, buffer);
            w->mark_dirty();

            if (w->on_text_change) {
                w->on_text_change(w->id, buffer, w->on_text_change_data);
            }
        }
        free(buffer);
        return;
    }
    
    
    if ((ev.character == 10 || ev.character == 13) && w->type == WidgetType::TextArea) {
        memcpy(buffer, current, len);
        buffer[len] = '\n';
        buffer[len + 1] = '\0';
        string_table_update(w->text, buffer);
        w->mark_dirty();

        if (w->on_text_change) {
            w->on_text_change(w->id, buffer, w->on_text_change_data);
        }
        free(buffer);
        return;
    }
    
    
    if (ev.character == 9 && w->type == WidgetType::TextArea) {
        memcpy(buffer, current, len);
        memcpy(buffer + len, "    ", 4); 
        buffer[len + 4] = '\0';
        string_table_update(w->text, buffer);
        w->mark_dirty();

        if (w->on_text_change) {
            w->on_text_change(w->id, buffer, w->on_text_change_data);
        }
        free(buffer);
        return;
    }

    
    if (ev.character >= 32 && ev.character < 127) {
        memcpy(buffer, current, len);
        buffer[len] = (char)ev.character;
        buffer[len + 1] = '\0';
        string_table_update(w->text, buffer);
        w->mark_dirty();

        if (w->on_text_change) {
            w->on_text_change(w->id, buffer, w->on_text_change_data);
        }
    }
    free(buffer);
}

} 
