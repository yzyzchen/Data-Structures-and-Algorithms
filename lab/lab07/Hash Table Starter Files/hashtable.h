#ifndef HASHTABLE_H
#define HASHTABLE_H

// Project Identifier: 2C4A3C53CD5AD45A7BEA3AE5356A3B1622C9D04B

// INSTRUCTIONS:
// fill out the methods in the class below.

// You may assume that the key and value types can be copied and have default
// constructors.

// You can also assume that the key type has (==) and (!=) methods.

// You may assume that Hasher is a functor type with a
// size_t operator()(const Key&) overload.

// The key/value types aren't guaranteed to support any other operations.

// Do not add, remove, or change any of the class's member variables.
// The num_deleted counter is *optional*. You might find it helpful, but it
// is not required to pass the lab assignment.

// Do not change the Bucket type.

// SUBMISSION INSTRUCTIONS:
// Submit this file, by itself, in a .tar.gz.
// Other files will be ignored.

#include <cstdint>
#include <functional> // where std::hash lives
#include <vector>
#include <cassert> // useful for debugging!

// A bucket's status tells you whether it's empty, occupied, 
// or contains a deleted element.
enum class Status : uint8_t {
    Empty,
    Occupied,
    Deleted
};

template<typename K, typename V, typename Hasher = std::hash<K>>
class HashTable {
    // used by the autograder; do not change/remove.
    friend class Verifier;
public:
    // A bucket has a status, a key, and a value.
    struct Bucket {
        // Do not modify Bucket.
        Status status = Status::Empty;
        K key;
        V val;
        
    };

    HashTable() {
        // TODO: a default constructor (possibly empty).

        // You can use the following to avoid implementing rehash_and_grow().
        // However, you will only pass the AG test cases ending in _C.
        // To pass the rest, start with at most 20 buckets and implement rehash_and_grow().
        buckets.resize(20);
    }

    size_t size() const {
        return num_elements;
    }

    // returns a reference to the value in the bucket with the key, if it
    // already exists. Otherwise, insert it with a default value, and return
    // a reference to the resulting bucket.
    // V& operator[](const K& key) {
    //     // TODO
    //     Hasher hasher;
    //     size_t desired_bucket = hasher(key) % buckets.size();
    //     size_t loc = checkBucket(desired_bucket, key);
    //     V val{};
    //     if (buckets[loc].status != Status::Occupied) {
    //         insert(key, val);
    //     }
    //     desired_bucket = hasher(key) % buckets.size();
    //     loc = checkBucket(desired_bucket, key);
    //     return buckets[loc].val;

    // }
    V& operator[](const K& key) {
        Hasher hasher;
        size_t desiredBucket = hasher(key) % buckets.size();
        size_t loc = checkBucket(desiredBucket, key);

        // If the bucket is occupied, it means the key exists, return its value.
        if (buckets[loc].status == Status::Occupied) {
            return buckets[loc].val;
        } else {
            // Key does not exist, insert it with a default value.
            V defaultValue{};
            insert(key, defaultValue);
            // Recalculate the bucket after potential rehashing.
            desiredBucket = hasher(key) % buckets.size();
            loc = checkBucket(desiredBucket, key);
            return buckets[loc].val;
        }
    }

    // insert returns whether inserted successfully
    // (if the key already exists in the table, do nothing and return false).
    bool insert(const K& key, const V& val) {
        // Before insertion, check if rehash is needed
        if (double(num_elements + 1) / double(buckets.size()) > 0.5) {
            rehash_and_grow();
        }

        Hasher hasher;
        size_t desired_bucket = hasher(key) % buckets.size();
        size_t loc = checkBucket(desired_bucket, key);

        // Check if the bucket is already occupied by the same key
        if (buckets[loc].status == Status::Occupied && buckets[loc].key == key) {
            return false; // Key already exists, do not insert
        }

        // Insert the key-value pair
        if (buckets[loc].status != Status::Occupied) {
            buckets[loc] = {Status::Occupied, key, val};
            num_elements++;
            return true;
        }

        return false;
    }
    // bool insert(const K& key, const V& val) {
    //     // TODO
    //     Hasher hasher;
    //     size_t desired_bucket = hasher(key) % buckets.size();
    //     size_t loc = checkBucket(desired_bucket, key);
    //     if (buckets[loc].status == Status::Occupied) return false;
    //     else{
    //         buckets[loc] = Bucket{Status::Occupied, key, val};
    //         ++num_elements;
    //     }
    //     //check if full
    //     if (double(num_elements) / double(buckets.size()) > 0.5) {
    //         rehash_and_grow();
    //     }
    //     return true;
    // }
    // erase returns the number of items remove (0 or 1)
    size_t erase(const K& key) {
        Hasher hasher;
        size_t desired_bucket = hasher(key) % buckets.size();
        size_t loc = checkBucket(desired_bucket, key);
        //if found nothing or full
        if (loc == buckets.size()) return 0;
        //if founded
        if (buckets[loc].status == Status::Occupied) {
            //change status to Deleted
            buckets[loc].status = Status::Deleted;
            num_elements--;
            return 1;
        }
        //else if empty
        return 0;
    }

private:
    size_t num_elements = 0;
    size_t num_deleted = 0; // OPTIONAL: you don't need to use num_deleted to pass
    std::vector<Bucket> buckets;

    // void rehash_and_grow() {
    //     // You can avoid implementing rehash_and_grow() by calling
    //     //    buckets.resize(10000);
    //     // in the constructor. However, you will only pass the AG test cases ending in _C.
    //     // To pass the rest, start with at most 20 buckets and implement rehash_and_grow().
    //     std::vector<Bucket> copy = buckets;
    //     buckets.clear();
    //     buckets.resize(copy.size() * 2);
    //     for (auto i : copy) {
    //         if (i.status == Status::Occupied)
    //             insert(i.key, i.val);
    //     }
    // }
    void rehash_and_grow() {
        std::vector<Bucket> old_buckets = std::move(buckets);
        buckets.resize(old_buckets.size() * 2, Bucket{Status::Empty});
        num_elements = 0; // Reset before re-inserting

        for (const auto& bucket : old_buckets) {
            if (bucket.status == Status::Occupied) {
                insert(bucket.key, bucket.val); // Re-insert
            }
        }
    }

    // You can add methods here if you like.
    // TODO

    // size_t checkBucket(size_t desiredBucket, const K& key) {
    //     for (size_t loops = 0; loops < buckets.size(); loops++) {
    //         Status bucketStatus = buckets[desiredBucket].status;

    //         // If the bucket is empty, it means the key is not in the table, and we can return this position.
    //         if (bucketStatus == Status::Empty) {
    //             return desiredBucket;
    //         }

    //         // If the bucket is occupied and the keys match, return the current bucket.
    //         if (bucketStatus == Status::Occupied && buckets[desiredBucket].key == key) {
    //             return desiredBucket;
    //         }

    //         // If the bucket is either deleted or occupied but with a different key, move to the next bucket.
    //         desiredBucket = (desiredBucket + 1) % buckets.size();
    //     }
    //     // If we complete the loop without returning, it means the table is full or a matching key was not found.
    //     return buckets.size();
    // }
    size_t checkBucket(size_t desiredBucket, const K& key) {
        size_t startIndex = desiredBucket, bucketCount = buckets.size();
        bool foundDeleted = false;
        size_t firstDeletedIndex = bucketCount; // Initialize with an invalid index.

        for (size_t loops = 0; loops < bucketCount; ++loops) {
            Status status = buckets[desiredBucket].status;

            if (status == Status::Empty) {
                // If a deleted bucket was found before, return its index to reuse it.
                return foundDeleted ? firstDeletedIndex : desiredBucket;
            }
            if (status == Status::Deleted && !foundDeleted) {
                // Mark that we've found a deleted bucket, which can be reused if the key isn't found.
                foundDeleted = true;
                firstDeletedIndex = desiredBucket;
            }
            if (status == Status::Occupied && buckets[desiredBucket].key == key) {
                // Key exists, return its bucket index.
                return desiredBucket;
            }

            desiredBucket = (desiredBucket + 1) % bucketCount; // Move to the next bucket.
        }

        // If the loop completes without finding an empty bucket or the key,
        return foundDeleted ? firstDeletedIndex : bucketCount;
    }
};

#endif // HASHTABLE_H