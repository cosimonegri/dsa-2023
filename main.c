#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/******* CONSTANTS AND TYPES *******/

#define COMMAND_SIZE 20
#define UNDEFINED 0

#define ADD_STATION "aggiungi-stazione"
#define DEL_STATION "demolisci-stazione"
#define ADD_CAR "aggiungi-auto"
#define DEL_CAR "rottama-auto"
#define FIND_PATH "pianifica-percorso"

#define ADDED "aggiunta\n"
#define NOT_ADDED "non aggiunta\n"
#define DEMOLISHED "demolita\n"
#define NOT_DEMOLISHED "non demolita\n"
#define SCRAPPED "rottamata\n"
#define NOT_SCRAPPED "non rottamata\n"
#define NO_PATH "nessun percorso\n"

#define VECTOR_INITIAL_SIZE 8
#define VECTOR_SIZE_MULTIPLIER 2

#define HT_INITIAL_SIZE 4
#define HT_SIZE_MULTIPLIER 2
#define HT_LOAD_FACTOR 1
#define HT_MAX_SIZE 512

typedef long INTEGER;

typedef struct SetNode SetNode;
typedef struct Set Set;
typedef struct HTNode HTNode;
typedef struct HashTable HashTable;
typedef struct StationCar StationCar;
typedef struct Vector Vector;

struct SetNode {
    INTEGER key;
    SetNode *next;
};

struct Set {
    SetNode **data;
    INTEGER size, used;
    SetNode *iterator;
    bool iterationFinished;
};

struct HTNode {
    INTEGER key;
    Set *value;
    HTNode *next;
};

struct HashTable {
    HTNode **data;
    INTEGER size, used;
    HTNode *iterator;
    bool iterationFinished;
};

struct StationCar {
    INTEGER station, car;
};

struct Vector {
    StationCar *data;
    INTEGER size, used;
};



/******* SET FUNCTION PROTOTYPES *******/

Set* setInit (INTEGER size);
INTEGER setBucketIdx (Set *set, INTEGER key);
SetNode** setBucket (Set *set, INTEGER key);
void setIter (Set *set);
SetNode* setNext (Set *set);
void setFree (Set *set);
bool setShouldResize (Set *set);
void setResize (Set *set);
SetNode* setSearch (Set *set, INTEGER key, SetNode **deleteHelper);
void setInsert (Set *set, INTEGER key);
void setInsertNode (Set *set, SetNode *node);
bool setDelete (Set *set, INTEGER key);



/******* HASH TABLE FUNCTION PROTOTYPES *******/

HashTable* htInit (INTEGER size);
INTEGER htBucketIdx (HashTable *ht, INTEGER key);
HTNode** htBucket (HashTable *ht, INTEGER key);
void htIter (HashTable *ht);
HTNode* htNext (HashTable *ht);
void htFree (HashTable *ht);
bool htShouldResize (HashTable *ht);
void htResize (HashTable *ht);
HTNode* htSearch (HashTable *ht, INTEGER key, HTNode **deleteHelper);
void htInsert (HashTable *ht, INTEGER key);
void htInsertNode (HashTable *ht, HTNode *node);
bool htDelete (HashTable *ht, INTEGER key);



/******* VECTOR FUNCTION PROTOTYPES *******/

Vector* vectorInit (INTEGER size);
void vectorFree (Vector *v);
void vectorResize (Vector *v);
INTEGER vectorLength (Vector *v);
StationCar* vectorGet (Vector *v, INTEGER idx);
INTEGER vectorGetStation (Vector *v, INTEGER idx);
INTEGER vectorGetCar (Vector *v, INTEGER idx);
void vectorSet (Vector *v, INTEGER idx, INTEGER station, INTEGER car);
void vectorPush (Vector *v, INTEGER station, INTEGER car);
INTEGER vectorFindStation (Vector *v, INTEGER station);
void vectorSortByStation (Vector *v);
int vectorCompareByStation (const void *data1, const void *data2);
INTEGER vectorGetStationsDist (Vector *v, INTEGER idx1, INTEGER idx2);



/******* OTHER FUNCTION PROTOTYPES *******/

void raiseCustomError (char *message);
bool isDigit (int character);
INTEGER readInt ();
bool addStation (HashTable *stations, INTEGER station);
void delStation (HashTable *stations, INTEGER station);
void addCar (HashTable *stations, INTEGER station, INTEGER car, bool print);
void delCar (HashTable *stations, INTEGER station, INTEGER car);
bool getPath (HashTable *stations, INTEGER start, INTEGER end, Vector *path);
INTEGER getScore (Vector* bestCars, INTEGER sourceIdx, INTEGER targetIdx);
bool getStraightPath (Vector *bestCars, INTEGER startIdx, INTEGER endIdx, Vector *path);
bool getReversedPath (Vector *bestCars, INTEGER startIdx, INTEGER endIdx, Vector *path);



/******* MAIN *******/

int main () {
    char command[COMMAND_SIZE];
    INTEGER counter, station, car, start, end;
    bool added, exists;

    HashTable *stations = htInit(HT_INITIAL_SIZE);
    Vector *path;

    while (scanf("%s", command) != EOF) {
        if (strcmp(command, ADD_STATION) == 0) {
            station = readInt();
            counter = readInt();
            added = addStation(stations, station);
            for (INTEGER i = 1; i <= counter; i++) {
                car = readInt();
                if (added) {
                    addCar(stations, station, car, false);
                }
            }
        }
        else if (strcmp(command, DEL_STATION) == 0) {
            station = readInt();
            delStation(stations, station);
        }
        else if (strcmp(command, ADD_CAR) == 0) {
            station = readInt();
            car = readInt();
            addCar(stations, station, car, true);
        }
        else if (strcmp(command, DEL_CAR) == 0) {
            station = readInt();
            car = readInt();
            delCar(stations, station, car);
        }
        else if (strcmp(command, FIND_PATH) == 0) {
            start = readInt();
            end = readInt();
            path = vectorInit(VECTOR_INITIAL_SIZE);
            exists = getPath(stations, start, end, path);
            if (exists) {
                for (INTEGER idx = 0; idx < path->used - 1; idx++) {
                    printf("%ld ", vectorGetStation(path, idx));
                }
                printf("%ld\n", vectorGetStation(path, path->used - 1));
            }
            else {
                printf(NO_PATH);
            }
            vectorFree(path);
        }
        else {
            raiseCustomError("unable to execute command");
        }
    }

    htFree(stations);
    return 0;
};



/******* OTHER FUNCTIONS *******/

void raiseCustomError (char *message) {
    printf("[ERROR]: %s\n", message);
    exit(EXIT_FAILURE);
};

bool isDigit (int character) {
    return character >= '0' && character <= '9';
}

INTEGER readInt () {
    INTEGER value = 0;
    int character = getchar_unlocked();
    while (!isDigit(character)) {
        character = getchar_unlocked();
    }
    while (isDigit(character)) {
        value = 10 * value + (character - '0');
        character = getchar_unlocked();
    }
    return value;
}

bool addStation (HashTable *stations, INTEGER station) {
    HTNode *node = htSearch(stations, station, NULL);
    if (node != NULL) {
        printf(NOT_ADDED);
        return false;
    };
    htInsert(stations, station);
    printf(ADDED);
    return true;
}

void delStation (HashTable *stations, INTEGER station) {
    bool deleted = htDelete(stations, station);
    if (deleted) {
        printf(DEMOLISHED);
    }
    else {
        printf(NOT_DEMOLISHED);
    }
}

void addCar (HashTable *stations, INTEGER station, INTEGER car, bool print) {
    HTNode *stationNode = htSearch(stations, station, NULL);
    if (stationNode == NULL) {
        if (print) {
            printf(NOT_ADDED);
        };
        return;
    }
    if (stationNode->value == NULL) {
        stationNode->value = setInit(HT_INITIAL_SIZE);
    }
    setInsert(stationNode->value, car);
    if (print) {
        printf(ADDED);
    };
}

void delCar (HashTable *stations, INTEGER station, INTEGER car) {
    HTNode *stationNode = htSearch(stations, station, NULL);
    if (stationNode == NULL || stationNode->value == NULL) {
        printf(NOT_SCRAPPED);
        return;
    }
    bool deleted = setDelete(stationNode->value, car);
    if (deleted) {
        printf(SCRAPPED);
    }
    else {
        printf(NOT_SCRAPPED);
    }
}

bool getPath (HashTable *stations, INTEGER start, INTEGER end, Vector *path) {
    if (start == end) {
        vectorPush(path, start, UNDEFINED);
        return true;
    }
    Vector *bestCars = vectorInit(stations->used);
    HTNode *station;
    Set *cars;
    SetNode *car;
    INTEGER bestCar;

    htIter(stations);
    for (station = htNext(stations); station; station = htNext(stations)) {
        bestCar = 0;
        cars = station->value;
        setIter(cars);
        for (car = setNext(cars); car; car = setNext(cars)) {
            if (car->key > bestCar) {
                bestCar = car->key;
            }
        }
        vectorPush(bestCars, station->key, bestCar);
    }
    vectorSortByStation(bestCars);

    INTEGER startIdx = vectorFindStation(bestCars, start);
    INTEGER endIdx = vectorFindStation(bestCars, end);
    vectorPush(path, start, UNDEFINED);
    bool exists;
    if (start < end) {
        exists = getStraightPath(bestCars, startIdx, endIdx, path);
    } else {
        exists = getReversedPath(bestCars, startIdx, endIdx, path);
    }
    vectorFree(bestCars);
    return exists;
};

INTEGER getScore (Vector* bestCars, INTEGER sourceIdx, INTEGER targetIdx) {
    return vectorGetStationsDist(bestCars, sourceIdx, targetIdx) + vectorGetCar(bestCars, targetIdx);
}

bool getStraightPath (Vector *bestCars, INTEGER startIdx, INTEGER endIdx, Vector *path) {
    INTEGER currIdx = startIdx, nextIdx, bestIdx;
    while (currIdx < endIdx) {
        nextIdx = currIdx + 1;
        bestIdx = -1;
        
        while (nextIdx <= endIdx && vectorGetStationsDist(bestCars, currIdx, nextIdx) <= vectorGetCar(bestCars, currIdx)) {
            if (bestIdx == -1 || nextIdx == endIdx || getScore(bestCars, currIdx, nextIdx) >= getScore(bestCars, currIdx, bestIdx)) {
                bestIdx = nextIdx;
            }
            nextIdx++;
        }
        if (bestIdx == -1) {
            return false;
        }
        vectorPush(path, vectorGetStation(bestCars, bestIdx), UNDEFINED);
        currIdx = bestIdx;
    }
    
    INTEGER targetPathIdx = vectorLength(path) - 1;
    INTEGER currPathIdx = vectorLength(path) - 2;
    currIdx = vectorFindStation(bestCars, vectorGetStation(path, currPathIdx));
    
    while (currPathIdx > 0) {
        INTEGER prevStation = vectorGetStation(path, currPathIdx - 1);
        
        while (vectorGetStation(bestCars, currIdx) > prevStation) {
            INTEGER car = vectorGetCar(bestCars, currIdx);
            if (vectorGetStation(path, targetPathIdx) - vectorGetStation(bestCars, currIdx) <= car) {
                vectorSet(path, currPathIdx, vectorGetStation(bestCars, currIdx), UNDEFINED);
            }
            currIdx--;
        }
        currPathIdx--;
        targetPathIdx--;
    }
    return true;
}

bool getReversedPath (Vector *bestCars, INTEGER startIdx, INTEGER endIdx, Vector *path) {
    INTEGER currIdx = startIdx, nextIdx, bestIdx;
    while (currIdx > endIdx) {
        nextIdx = currIdx - 1;
        bestIdx = -1;
        
        while (nextIdx >= endIdx && vectorGetStationsDist(bestCars, currIdx, nextIdx) <= vectorGetCar(bestCars, currIdx)) {
            if (bestIdx == -1 || nextIdx == endIdx || getScore(bestCars, currIdx, nextIdx) >= getScore(bestCars, currIdx, bestIdx)) {
                bestIdx = nextIdx;
            }
            nextIdx--;
        }
        if (bestIdx == -1) {
            return false;
        }
        vectorPush(path, vectorGetStation(bestCars, bestIdx), UNDEFINED);
        currIdx = bestIdx;
    }
    
    INTEGER currPathIdx = vectorLength(path) - 2;
    INTEGER prevPathIdx, nextPathIdx;
    INTEGER prevIdx, newIdx;
    bool changed;
    
    while (currPathIdx > 0) {
        nextPathIdx = currPathIdx + 1;
        prevPathIdx = currPathIdx - 1;
        
        prevIdx = vectorFindStation(bestCars, vectorGetStation(path, prevPathIdx));
        currIdx = vectorFindStation(bestCars, vectorGetStation(path, currPathIdx));
        nextIdx = vectorFindStation(bestCars, vectorGetStation(path, nextPathIdx));
        
        changed = false;
        
        newIdx = currIdx - 1;
        while (newIdx > nextIdx) {
            if (vectorGetStationsDist(bestCars, prevIdx, newIdx) <= vectorGetCar(bestCars, prevIdx) && vectorGetStationsDist(bestCars, newIdx, nextIdx) <= vectorGetCar(bestCars, newIdx)) {
                vectorSet(path, currPathIdx, vectorGetStation(bestCars, newIdx), UNDEFINED);
                changed = true;
            }
            newIdx--;
        }

        if (changed) {
            currPathIdx = vectorLength(path) - 2;
        }
        else {
            currPathIdx--;
        }
    }
    return true;
}



/******* SET FUNCTIONS *******/

Set* setInit (INTEGER size) {
    Set *set = malloc(sizeof(Set));
    set->data = malloc(size * sizeof(SetNode*));
    set->size = size;
    set->used = 0;
    for (INTEGER bucketIdx = 0; bucketIdx < size; bucketIdx++) {
        set->data[bucketIdx] = NULL;
    }
    return set;
};

INTEGER setBucketIdx (Set *set, INTEGER key) {
    return key % set->size;
}

SetNode** setBucket (Set *set, INTEGER key) {
    INTEGER index = setBucketIdx(set, key);
    return set->data + index;
}

void setIter (Set *set) {
    if (set == NULL) {
        return;
    }
    set->iterator = NULL;
    set->iterationFinished = false;
}

SetNode* setNext (Set *set) {
    if (set == NULL || set->iterationFinished) {
        return NULL;
    }
    if (set->iterator != NULL && set->iterator->next != NULL) {
        set->iterator = set->iterator->next;
        return set->iterator;
    }
    SetNode *next;
    INTEGER bucketIdx;
    if (set->iterator == NULL) {
        bucketIdx = 0;
    } else {
        bucketIdx = setBucketIdx(set, set->iterator->key) + 1;
    }
    for (; bucketIdx < set->size; bucketIdx++) {
        next = set->data[bucketIdx];
        if (next != NULL) {
            set->iterator = next;
            return set->iterator;
        }
    }
    set->iterator = NULL;
    set->iterationFinished = true;
    return NULL;
}

void setFree (Set *set) {
    if (set == NULL) {
        return;
    }
    setIter(set);
    SetNode *node = setNext(set);
    SetNode *temp = NULL;
    while (node != NULL) {
        temp = setNext(set);
        free(node);
        node = temp;
    }
    free(set->data);
    free(set);
}

bool setShouldResize (Set *set) {
    if (set->size >= HT_MAX_SIZE) {
        return false;
    }
    return set->used >= HT_LOAD_FACTOR * set->size;
}

void setResize (Set *set) {
    Set *tempHt = setInit(HT_SIZE_MULTIPLIER * set->size);
    setIter(set);
    SetNode *node = setNext(set);
    SetNode *temp = NULL;
    while (node != NULL) {
        temp = setNext(set);
        setInsertNode(tempHt, node);
        node = temp;
    }
    free(set->data);
    set->data = tempHt->data;
    set->size = tempHt->size;
    set->used = tempHt->used;
    free(tempHt);
};

SetNode* setSearch (Set *set, INTEGER key, SetNode **deleteHelper) {
    SetNode *prev = NULL;
    SetNode *node = *setBucket(set, key);
    while (node != NULL && node->key != key) {
        prev = node;
        node = node->next;
    }
    if (deleteHelper != NULL) {
        *deleteHelper = prev;
    }
    return node;
};

void setInsert (Set *set, INTEGER key) {
    SetNode *node = setSearch(set, key, NULL);
    if (node != NULL) {
        return;
    }
    node = malloc(sizeof(SetNode));
    node->key = key;
    node->next = NULL;
    if (setShouldResize(set)) {
        setResize(set);
    }
    setInsertNode(set, node);
}

void setInsertNode (Set *set, SetNode *node) {
    SetNode **bucket = setBucket(set, node->key);
    node->next = *bucket;
    *bucket = node;
    set->used++;
}

bool setDelete (Set *set, INTEGER key) {
    SetNode *prev = NULL;
    SetNode *node = setSearch(set, key, &prev);
    if (node == NULL) {
        return false;
    }
    if (prev == NULL) {
        SetNode **bucket = setBucket(set, key);
        *bucket = node->next;
    }
    else {
        prev->next = node->next;
    }
    free(node);
    set->used--;
    return true;
};



/******* HASH TABLE FUNCTIONS *******/

HashTable* htInit (INTEGER size) {
    HashTable *ht = malloc(sizeof(HashTable));
    ht->data = malloc(size * sizeof(HTNode*));
    ht->size = size;
    ht->used = 0;
    for (INTEGER bucketIdx = 0; bucketIdx < size; bucketIdx++) {
        ht->data[bucketIdx] = NULL;
    }
    return ht;
};

INTEGER htBucketIdx (HashTable *ht, INTEGER key) {
    return key % ht->size;
}

HTNode** htBucket (HashTable *ht, INTEGER key) {
    INTEGER index = htBucketIdx(ht, key);
    return ht->data + index;
}

void htIter (HashTable *ht) {
    if (ht == NULL) {
        return;
    }
    ht->iterator = NULL;
    ht->iterationFinished = false;
}

HTNode* htNext (HashTable *ht) {
    if (ht == NULL || ht->iterationFinished) {
        return NULL;
    }
    if (ht->iterator != NULL && ht->iterator->next != NULL) {
        ht->iterator = ht->iterator->next;
        return ht->iterator;
    }
    HTNode *next;
    INTEGER bucketIdx;
    if (ht->iterator == NULL) {
        bucketIdx = 0;
    } else {
        bucketIdx = htBucketIdx(ht, ht->iterator->key) + 1;
    }
    for (; bucketIdx < ht->size; bucketIdx++) {
        next = ht->data[bucketIdx];
        if (next != NULL) {
            ht->iterator = next;
            return ht->iterator;
        }
    }
    ht->iterator = NULL;
    ht->iterationFinished = true;
    return NULL;
}

void htFree (HashTable *ht) {
    if (ht == NULL) {
        return;
    }
    htIter(ht);
    HTNode *node = htNext(ht);
    HTNode *temp = NULL;
    while (node != NULL) {
        temp = htNext(ht);
        setFree(node->value);
        free(node);
        node = temp;
    }
    free(ht->data);
    free(ht);
}

bool htShouldResize (HashTable *ht) {
    return ht->used >= HT_LOAD_FACTOR * ht->size;
}

void htResize (HashTable *ht) {
    HashTable *tempHt = htInit(HT_SIZE_MULTIPLIER * ht->size);
    htIter(ht);
    HTNode *node = htNext(ht);
    HTNode *temp = NULL;
    while (node != NULL) {
        temp = htNext(ht);
        htInsertNode(tempHt, node);
        node = temp;
    }
    free(ht->data);
    ht->data = tempHt->data;
    ht->size = tempHt->size;
    ht->used = tempHt->used;
    free(tempHt);
};

HTNode* htSearch (HashTable *ht, INTEGER key, HTNode **deleteHelper) {
    HTNode *prev = NULL;
    HTNode *node = *htBucket(ht, key);
    while (node != NULL && node->key != key) {
        prev = node;
        node = node->next;
    }
    if (deleteHelper != NULL) {
        *deleteHelper = prev;
    }
    return node;
};

void htInsert (HashTable *ht, INTEGER key) {
    HTNode *node = htSearch(ht, key, NULL), *newNode;
    if (node != NULL) {
        return;
    }
    newNode = malloc(sizeof(HTNode));
    newNode->key = key;
    newNode->value = NULL;
    newNode->next = NULL;
    if (htShouldResize(ht)) {
        htResize(ht);
    }
    htInsertNode(ht, newNode);
}

void htInsertNode (HashTable *ht, HTNode *node) {
    if (node == NULL) {
        return;
    }
    HTNode **bucket = htBucket(ht, node->key);
    if (bucket == NULL) {
        return;
    }
    node->next = *bucket;
    *bucket = node;
    ht->used++;
}

bool htDelete (HashTable *ht, INTEGER key) {
    HTNode *prev = NULL;
    HTNode *node = htSearch(ht, key, &prev);
    if (node == NULL) {
        return false;
    }
    if (prev == NULL) {
        HTNode **bucket = htBucket(ht, key);
        *bucket = node->next;
    }
    else {
        prev->next = node->next;
    }
    setFree(node->value);
    free(node);
    ht->used--;
    return true;
};



/******* VECTOR FUNCTIONS *******/

Vector* vectorInit (INTEGER size) {
    Vector *v = malloc(sizeof(Vector));
    v->data = malloc(size * sizeof(StationCar));
    v->size = size;
    v->used = 0;
    return v;
}
void vectorFree (Vector *v) {
    free(v->data);
    free(v);
}
void vectorResize (Vector *v) {
    INTEGER newSize = VECTOR_SIZE_MULTIPLIER * v->size;
    v->data = realloc(v->data, newSize * sizeof(StationCar));
    v->size = newSize;
}
INTEGER vectorLength (Vector *v) {
    return v->used;
}
StationCar* vectorGet (Vector *v, INTEGER idx) {
    if (idx < 0 || idx > vectorLength(v)) {
        raiseCustomError("invalid index (get)");
        return NULL;
    }
    return v->data + idx;
}
INTEGER vectorGetStation (Vector *v, INTEGER idx) {
    StationCar *node = vectorGet(v, idx);
    if (node) {
        return node->station;
    } else {
        return -1;
    }
}
INTEGER vectorGetCar (Vector *v, INTEGER idx) {
    StationCar *node = vectorGet(v, idx);
    if (node) {
        return node->car;
    } else {
        return -1;
    }
}
void vectorSet (Vector *v, INTEGER idx, INTEGER station, INTEGER car) {
    if (idx < 0 || idx > vectorLength(v)) {
        raiseCustomError("invalid index (set)");
        return;
    }
    if (vectorLength(v) == v->size) {
        vectorResize(v);
    }
    v->data[idx].station = station;
    v->data[idx].car = car;
    if (idx == vectorLength(v)) {
        v->used++;
    }
}
void vectorPush (Vector *v, INTEGER station, INTEGER car) {
    vectorSet(v, vectorLength(v), station, car);
}
INTEGER vectorFindStation (Vector *v, INTEGER station) {
    for (INTEGER idx = 0; idx < vectorLength(v); idx++) {     
        if (vectorGetStation(v, idx) == station) {
            return idx;
        }     
    }
    raiseCustomError("unable to find vector idx");
    return -1;
}
void vectorSortByStation (Vector *v) {
    qsort(v->data, vectorLength(v), sizeof(StationCar), vectorCompareByStation);
}
int vectorCompareByStation (const void *data1, const void *data2) {
    StationCar *node1 = (StationCar*) data1;
    StationCar *node2 = (StationCar*) data2;
    return node1->station - node2->station;
}
INTEGER vectorGetStationsDist (Vector *v, INTEGER idx1, INTEGER idx2) {
    INTEGER station1 = vectorGetStation(v, idx1);
    INTEGER station2 = vectorGetStation(v, idx2);
    return abs(station2 - station1);
}
