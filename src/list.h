#include <Arduino.h>

template <typename T, size_t MAX_ITEMS>
class List {
public:
    List() : itemCount(0) {}

    bool add(const T &item) {
        if (itemCount < MAX_ITEMS) {
            items[itemCount++] = item;
            return true; // Added successfully
        }
        return false; // List is full
    }

    void remove(const T &item) {
        for (size_t i = 0; i < itemCount; i++) {
            if (items[i] == item) {
                // Shift items to fill the gap
                for (size_t j = i; j < itemCount - 1; j++) {
                    items[j] = items[j + 1];
                }
                itemCount--;
                break; // Item found and removed
            }
        }
    }

    bool contains(const T &item) const {
        for (size_t i = 0; i < itemCount; i++) {
            if (items[i] == item) {
                return true;
            }
        }
        return false;
    }

    void clear() {
        itemCount = 0;
    }

    size_t size() const {
        return itemCount;
    }

protected:
    T items[MAX_ITEMS];
    size_t itemCount;
};