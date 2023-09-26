#include <Arduino.h>

/**
 * @brief A simple list (array) with a fixed maximum number of items.
 *
 * @tparam T The type of items to store in the list.
 * @tparam MAX_ITEMS The maximum number of items the list can hold.
 */
template <typename T, size_t MAX_ITEMS>
class List {
public:
    List() : itemCount(0) {}

    /**
     * @brief Add an item to the list.
     *
     * @param item The item to add.
     * @return true if the item was added successfully, false if the list is full.
     */
    bool add(const T &item) {
        if (itemCount < MAX_ITEMS) {
            items[itemCount++] = item;
            return true; // Added successfully
        }
        return false; // List is full
    }

    /**
     * @brief Remove an item from the list.
     *
     * @param item The item to remove.
     */
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

    /**
     * @brief Check if the list contains a specific item.
     *
     * @param item The item to check for.
     * @return true if the item is found in the list, false otherwise.
     */
    bool contains(const T &item) const {
        for (size_t i = 0; i < itemCount; i++) {
            if (items[i] == item) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Clear all items from the list.
     */
    void clear() {
        itemCount = 0;
    }

    /**
     * @brief Get the number of items currently in the list.
     *
     * @return The number of items in the list.
     */
    size_t size() const {
        return itemCount;
    }

protected:
    T items[MAX_ITEMS];
    size_t itemCount;
};
