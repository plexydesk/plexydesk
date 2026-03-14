#pragma once

#include "widget.hpp"
#include <cstdint>

namespace plexy::ui {



enum class UIEventType : uint8_t {
    MouseMove,
    MouseButton,
    MouseScroll,
    KeyDown,
    KeyUp,
    CharInput,
};

struct UIEvent {
    UIEventType type;
    float x, y;
    int32_t button;
    bool pressed;
    float scroll_dx, scroll_dy;
    uint32_t keycode;
    uint32_t modifiers;
    uint32_t character;
};



class EventQueue {
public:
    void push(const UIEvent& event);
    bool pop(UIEvent* out);
    bool empty() const;
    void clear();

private:
    static constexpr uint32_t QUEUE_SIZE = 256;
    UIEvent buffer_[QUEUE_SIZE];
    uint32_t head_ = 0;
    uint32_t tail_ = 0;
    uint32_t count_ = 0;
    bool has_pending_move_ = false;
    uint32_t pending_move_idx_ = 0;
};



class SpatialGrid {
public:
    void init(int screen_w, int screen_h);
    void shutdown();
    void rebuild(Widget* root);
    Widget* hit_test(float x, float y);

private:
    static constexpr int CELL_SIZE = 32;
    static constexpr int MAX_PER_CELL = 8;

    struct Cell {
        Widget* widgets[MAX_PER_CELL];
        uint8_t count;
    };

    Cell* cells_ = nullptr;
    int cols_ = 0, rows_ = 0;
    int screen_w_ = 0, screen_h_ = 0;

    void insert_widget(Widget* w);
    Cell* cell_at(int col, int row);
};



class FocusManager {
public:
    Widget* focused() const { return focused_; }
    void set_focus(Widget* w);
    void clear_focus();
    void focus_next(Widget* root);
    void focus_prev(Widget* root);

private:
    Widget* focused_ = nullptr;
    static constexpr int MAX_FOCUSABLE = 512;
    Widget* focusable_[MAX_FOCUSABLE];
    uint32_t focusable_count_ = 0;
    void collect_focusable(Widget* root);
    int find_index(Widget* w);
};



class EventProcessor {
public:
    bool process_all(EventQueue* queue, SpatialGrid* grid,
                     FocusManager* focus, Widget* root);

private:
    Widget* hovered_ = nullptr;
    Widget* pressed_ = nullptr;
    float press_x_ = 0, press_y_ = 0;

    bool handle_mouse_move(const UIEvent& ev, SpatialGrid* grid, FocusManager* focus);
    void handle_mouse_button(const UIEvent& ev, SpatialGrid* grid, FocusManager* focus);
    void handle_mouse_scroll(const UIEvent& ev, SpatialGrid* grid);
    void handle_key(const UIEvent& ev, FocusManager* focus);
    void handle_char_input(const UIEvent& ev, FocusManager* focus);
    void set_hovered(Widget* w);
};

} 
