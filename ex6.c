/******************
Name: Ariel Mizrahi
ID:
Assignment: ex6
*******************/
#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str) {
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0) {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int) strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src) {
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *) malloc(len + 1);
    if (!dest) {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt) {
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success) {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0) {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int) strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0') {
            printf("Invalid input.\n");
        } else {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type) {
    switch (type) {
        case GRASS:
            return "GRASS";
        case FIRE:
            return "FIRE";
        case WATER:
            return "WATER";
        case BUG:
            return "BUG";
        case NORMAL:
            return "NORMAL";
        case POISON:
            return "POISON";
        case ELECTRIC:
            return "ELECTRIC";
        case GROUND:
            return "GROUND";
        case FAIRY:
            return "FAIRY";
        case FIGHTING:
            return "FIGHTING";
        case PSYCHIC:
            return "PSYCHIC";
        case ROCK:
            return "ROCK";
        case GHOST:
            return "GHOST";
        case DRAGON:
            return "DRAGON";
        case ICE:
            return "ICE";
        default:
            return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput() {
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *) malloc(capacity);
    if (!input) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (size + 1 >= capacity) {
            capacity *= 2;
            char *temp = (char *) realloc(input, capacity);
            if (!temp) {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char) c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// --------------------------------------------------------------
// Utility: isIDExist checks if a given ID already exists in the pokedex
// --------------------------------------------------------------
int isIDExist(PokemonNode *root, int id) {
    if (root == NULL) {
        return 0;
    }
    if (root->data->id == id) {
        return 1;
    }
    if (root->data->id > id) {
        return isIDExist(root->left, id);
    }
    return isIDExist(root->right, id);
}

// --------------------------------------------------------------
// Utility: printPokemonNode prints a single Pokemon node
// --------------------------------------------------------------

void printPokemonNode(PokemonNode *node) {
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// functions for creating a queue of pokemone nodes to help the bfsgeneric function
// --------------------------------------------------------------
node *createNode(PokemonNode *data) {
    node *myNode = (node *) malloc(sizeof(node));
    if (!myNode) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    myNode->data = data;
    myNode->next = NULL;
    return myNode;
}

queue *createQueue() {
    queue *myq = (queue *) malloc(sizeof(queue));
    if (!myq) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    myq->front = NULL;
    myq->rear = NULL;
    return myq;
}

int isEmpty(queue *myq) {
    return myq->front == NULL;
}

void enQueue(queue *myq, PokemonNode *data) {
    node *myNode = createNode(data);
    if (myq->rear == NULL) {
        myq->front = myNode;
        myq->rear = myNode;
        return;
    }
    myq->rear->next = myNode;
    myq->rear = myNode;
}

PokemonNode *deQueue(queue *myq) {
    if (isEmpty(myq)) {
        printf("Queue is empty.\n");
        return NULL;
    }
    node *myNode = myq->front;
    myq->front = myq->front->next;
    if (myq->front == NULL) {
        myq->rear = NULL;
    }
    PokemonNode *deletedNode = myNode->data;
    free(myNode);
    return deletedNode;
}

// --------------------------------------------------------------
// the generic function the activates the visit func in their respected order
// --------------------------------------------------------------

// we use visit func on each node in the tree in a BFS way, using a queue
void BFSGeneric(PokemonNode *root, VisitNodeFunc visit) {
    queue *myq = createQueue();
    enQueue(myq, root);
    while (!isEmpty(myq)) {
        PokemonNode *current = deQueue(myq);
        visit(current);
        if (current->left != NULL)
            enQueue(myq, current->left);
        if (current->right != NULL)
            enQueue(myq, current->right);
    }
    free(myq);
}

// we use visit func on each node in the tree in a pre order way
void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        return;
    }
    visit(root);
    preOrderGeneric(root->left, visit);
    preOrderGeneric(root->right, visit);
}

// we use visit func on each node in the tree in an in order way
void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        return;
    }
    inOrderGeneric(root->left, visit);
    visit(root);
    inOrderGeneric(root->right, visit);
}

// we use visit func on each node in the tree in a post order way
void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        return;
    }
    postOrderGeneric(root->left, visit);
    postOrderGeneric(root->right, visit);
    visit(root);
}

// --------------------------------------------------------------
// Display functions that call their matching generic function with  print functions
// --------------------------------------------------------------
void displayBFS(PokemonNode *root) {
    if (root == NULL) {
        return;
    }
    BFSGeneric(root, printPokemonNode);
}

void preOrderTraversal(PokemonNode *root) {
    if (root == NULL) {
        return;
    }
    preOrderGeneric(root, printPokemonNode);
}

void inOrderTraversal(PokemonNode *root) {
    if (root == NULL) {
        return;
    }
    inOrderGeneric(root, printPokemonNode);
}

void postOrderTraversal(PokemonNode *root) {
    if (root == NULL) {
        return;
    }
    postOrderGeneric(root, printPokemonNode);
}

void displayAlphabetical(PokemonNode *root) {
    // gets how much nodes are in the tree
    int capacity = getCapacity(root);
    NodeArray pokenodes;
    // creates node array with the size of the capacity
    initNodeArray(&pokenodes, capacity);
    // take all the nodes and insert into the node array
    collectAll(&pokenodes, root);
    // sort them alphabetically and print
    qsort(pokenodes.nodes, pokenodes.size, sizeof(PokemonNode *), compareByNameNode);
    for (int i = 0; i < capacity; i++) {
        printPokemonNode(pokenodes.nodes[i]);
    }
    free(pokenodes.nodes);
}

// --------------------------------------------------------------
// support functions for the displayAlphabetical func
// --------------------------------------------------------------

// getCapacity gets the number of nodes in the tree
int getCapacity(PokemonNode *root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + getCapacity(root->left) + getCapacity(root->right);
}

// initialize the node array and allocate him the size of the capacity
void initNodeArray(NodeArray *na, int capacity) {
    na->nodes = malloc(capacity * sizeof(PokemonNode *));
    if (!na->nodes) {
        printf("Memory allocation failed.\n");
        free(na->nodes);
        exit(1);
    }
    na->capacity = capacity;
    na->size = 0;
}

/* adds new note to the node array with a given pokemon node in the place
size which is initialized at 0 and goes up by 1 every func call*/
void addNode(NodeArray *na, PokemonNode *node) {
    na->nodes[na->size] = node;
    na->size++;
}

// collectAll enters all the pokemon nodes in to the node array
void collectAll(NodeArray *na, PokemonNode *root) {
    if (root == NULL) {
        return;
    }
    addNode(na, root);
    collectAll(na, root->left);
    collectAll(na, root->right);
}

// generic func for qsort that compares pokemon names using strcmp
int compareByNameNode(const void *a, const void *b) {
    PokemonNode *node1 = *(PokemonNode **) a;
    PokemonNode *node2 = *(PokemonNode **) b;
    return strcmp(node1->data->name, node2->data->name);
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner) {
    if (!owner->pokedexRoot) {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice) {
        case 1:
            displayBFS(owner->pokedexRoot);
            break;
        case 2:
            preOrderTraversal(owner->pokedexRoot);
            break;
        case 3:
            inOrderTraversal(owner->pokedexRoot);
            break;
        case 4:
            postOrderTraversal(owner->pokedexRoot);
            break;
        case 5:
            displayAlphabetical(owner->pokedexRoot);
            break;
        default:
            printf("Invalid choice.\n");
    }
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu() {
    if (ownerHead == NULL) {
        printf("No existing Pokedexes.\n");
        return;
    }
    // list owners
    printf("\nExisting Pokedexes:\n");
    // you need to implement a few things here :)
    printOwnersList();
    int id = readIntSafe("Choose a Pokedex by number: ");
    OwnerNode *cur = getOwnerByID(id);
    printf("\nEntering %s's Pokedex...\n", cur->ownerName);
    int subChoice;
    do {
        printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice) {
            case 1:
                addPokemon(cur);
                break;
            case 2:
                displayMenu(cur);
                break;
            case 3:
                freePokemon(cur);
                break;
            case 4:
                pokemonFight(cur);
                break;
            case 5:
                evolvePokemon(cur);
                break;
            case 6:
                printf("Back to Main Menu.\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

// --------------------------------------------------------------
// add Pokemon funcs
// --------------------------------------------------------------
// adds pokemon to the owner pokedex
void addPokemon(OwnerNode *owner) {
    int id = readIntSafe("Enter ID to add: ");
    // first checking if the id already exists if so leaving the func before making changes
    if (isIDExist(owner->pokedexRoot, id)) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", id);
        return;
    }
    // if the id doesn't exist we create a new pokemon node and giving it the value of the chosen pokemon
    PokemonNode *newNode = createPokemonNode(&pokedex[id - 1]);
    // and then inserting the node in to the pokedex
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newNode);
    printf("Pokemon %s (ID %d) added.", newNode->data->name, newNode->data->id);
}

/* insertPokemonNode take a root and a new node that is to be placed in the
pokedex and placing the node in the correct place of the bst(by id)*/
PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode) {
    if (root == NULL) {
        return newNode;
    }
    // finds the correct location of the pokemon node by traversing the tree left and right
    if (root->data->id > newNode->data->id) {
        root->left = insertPokemonNode(root->left, newNode);
    }
    if (root->data->id < newNode->data->id) {
        root->right = insertPokemonNode(root->right, newNode);
    }
    return root;
}

// --------------------------------------------------------------
// free Pokemon funcs
// --------------------------------------------------------------
void freePokemonNode(PokemonNode *node) {
    if (node == NULL) {
        return;
    }
    free(node);
}

PokemonNode *findMinNode(PokemonNode *root) {
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

// Remove a node with a given id
PokemonNode *removeNodeById(PokemonNode *root, int id) {
    // empty pokedex
    if (!root) {
        return NULL;
    }
    if (root->data->id > id) {
        root->left = removeNodeById(root->left, id);
    } else if (root->data->id < id) {
        root->right = removeNodeById(root->right, id);
    } else {
        // No smaller children, switch with the right one
        if (root->left == NULL) {
            PokemonNode *temp = root->right;
            freePokemonNode(root);

            return temp;
            // No bigger children, switch with the left one
        } else if (root->right == NULL) {
            PokemonNode *temp = root->left;
            freePokemonNode(root);
            return temp;
        }
        /* When there are two child we need to swap the children we want to delete
        with the child with the smallest id of his *right side tree*/
        PokemonNode *temp = findMinNode(root->right);
        root->data = temp->data;
        root->right = removeNodeById(root->right, temp->data->id);
    }
    return root;
}

// freepokemon gets the id to remove and calls removeNodeById
void freePokemon(OwnerNode *owner) {
    // checks if the pokedex is empty
    if (!owner->pokedexRoot) {
        printf("No Pokemon to release.\n");
        return;
    }
    int id = readIntSafe("Enter Pokemon ID to release: ");
    // check if the id of the pokemon even exist
    if (!isIDExist(owner->pokedexRoot, id)) {
        printf("No Pokemon with ID %d found.", id);
        return;
    }
    printf("Removing Pokemon %s (ID %d).\n", pokedex[id - 1].name, id);
    owner->pokedexRoot = removeNodeById(owner->pokedexRoot, id);
}

// --------------------------------------------------------------
// Pokemon fight funcs
// --------------------------------------------------------------
// pokemon fight calculates the score of each pokemon and prints the winner
void pokemonFight(OwnerNode *owner) {
    if (!owner->pokedexRoot) {
        printf(" Pokedex is empty.");
        return;
    }
    int id1 = readIntSafe("Enter ID of the first Pokemon: ");
    int id2 = readIntSafe("Enter ID of the second Pokemon: ");
    if (!isIDExist(owner->pokedexRoot, id1) && !isIDExist(owner->pokedexRoot, id2)) {
        printf("One or both Pokemon IDs not found.");
        return;
    }
    int attack1 = pokedex[id1 - 1].attack;
    int hp1 = pokedex[id1 - 1].hp;
    int attack2 = pokedex[id2 - 1].attack;
    int hp2 = pokedex[id2 - 1].hp;
    float score1 = attack1 * 1.5 + hp1 * 1.2;
    float score2 = attack2 * 1.5 + hp2 * 1.2;
    printf("Pokemon 1: %s (Score = %.2f)\n", pokedex[id1 - 1].name, score1);
    printf("Pokemon 2: %s (Score = %.2f)\n", pokedex[id2 - 1].name, score2);
    if (score1 > score2) {
        printf("%s wins!\n", pokedex[id1 - 1].name);
    } else if (score2 > score1) {
        printf("%s wins!\n", pokedex[id2 - 1].name);
    } else {
        printf("It's a tie!\n");
    }
}

// --------------------------------------------------------------
// evolve Pokemon funcs
// --------------------------------------------------------------
void evolvePokemon(OwnerNode *owner) {
    if (!owner->pokedexRoot) {
        // If pokedex is empty
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }
    int idToEvo = readIntSafe("Enter ID of Pokemon to evolve: ");
    // if id of the pokemon we want to evolve doesn't exist
    if (!isIDExist(owner->pokedexRoot, idToEvo)) {
        printf("No Pokemon with ID %d found.\n", idToEvo);
        return;
    }
    // If the pokemon cannot evolve
    if (!pokedex[idToEvo - 1].CAN_EVOLVE) {
        printf("%s (ID %d) cannot evolve.\n", pokedex[idToEvo - 1].name, idToEvo);
        return;
    }
    //if the evolved pokemon already exist
    if (isIDExist(owner->pokedexRoot, idToEvo + 1)) {
        printf("Evolution ID %d (%s) already in the Pokedex. Releasing %s (ID %d).\n", (idToEvo + 1),
               pokedex[idToEvo].name, pokedex[idToEvo - 1].name, idToEvo);

        owner->pokedexRoot = removeNodeById(owner->pokedexRoot, idToEvo);
        return;
    }
    /* else means all the conditions are met and we can evolve (by deleting the current
    pokemon and adding his evolve form*/
    else {
        removeNodeById(owner->pokedexRoot, idToEvo);
        PokemonNode *newEvoNode = createPokemonNode(&pokedex[idToEvo]);
        owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newEvoNode);
        printf("Pokemon evolved from %s (ID %d) to %s(ID %d).\n", pokedex[idToEvo - 1].name, idToEvo,
               pokedex[idToEvo].name, (idToEvo + 1));
    }
}

// --------------------------------------------------------------
// Delete owner funcs
// --------------------------------------------------------------
void freeOwnerNode(OwnerNode *owner) {
    if (owner == NULL) {
        return;
    }
    free(owner->ownerName);
    freePokemonTree(owner->pokedexRoot);
    free(owner);
}

void freePokemonTree(PokemonNode *root) {
    if (root == NULL) {
        return;
    }
    freePokemonTree(root->left);
    freePokemonTree(root->right);
    freePokemonNode(root);
}

OwnerNode *getOwnerByID(int id) {
    OwnerNode *node = ownerHead;
    for (int i = 1; i < id; i++) {
        node = node->next;
    }
    return node;
}

// wrapper func that calls removeOwnerFromCircularList and prints things
void deletePokedex() {
    if (ownerHead == NULL) {
        printf("No existing Pokedexes to delete.\n");
        return;
    }
    printf("\n=== Delete a Pokedex ===\n");
    printOwnersList();
    int id = readIntSafe("Choose a Pokedex to delete by number: ");
    OwnerNode *ownerToDelete = getOwnerByID(id);
    printf("Deleting %s's entire Pokedex...\n", ownerToDelete->ownerName);
    removeOwnerFromCircularList(ownerToDelete);
    printf("Pokedex deleted.\n");
}

void removeOwnerFromCircularList(OwnerNode *target) {
    // If there is only one owner
    if (target == target->next) {
        ownerHead = NULL;
        return;
    }
    // If we delete the first node we need ownerHead to point to the next node
    if (ownerHead == target) {
        target->next->prev = target->prev;
        target->prev->next = target->next;
        ownerHead = target->next;
        freeOwnerNode(target);
        return;
    }
    // otherwise link the prev of the target with the next of the target on both ends
    target->next->prev = target->prev;
    target->prev->next = target->next;
    freeOwnerNode(target);
}

// --------------------------------------------------------------
// merge pokedex funcs
// --------------------------------------------------------------

// Merging pokedexes of 2 owners using a bfs logic with a queue
void mergePokedexBFS(OwnerNode *firstOwner, OwnerNode *secondOwner) {
    queue *myq = createQueue();
    enQueue(myq, secondOwner->pokedexRoot);
    while (!isEmpty(myq)) {
        PokemonNode *current = deQueue(myq);
        // creating a temp so that when we free the second pokedex we won't lose all the pointers
        PokemonNode *temp = createPokemonNode(current->data);
        insertPokemonNode(firstOwner->pokedexRoot, temp);
        if (current->left != NULL) {
            enQueue(myq, current->left);
        }
        if (current->right != NULL) {
            enQueue(myq, current->right);
        }
    }
    free(myq);
}

void mergePokedexMenu() {
    // 0 or 1 owners, cant merge
    if (ownerHead == NULL || ownerHead->next == ownerHead) {
        printf("Not enough owners to merge.\n");
        return;
    }
    printf("\n=== Merge Pokedexes ===\n");
    printf("Enter name of first owner: ");
    char *firstname = getDynamicInput();
    printf("Enter name of second owner: ");
    char *secondname = getDynamicInput();
    // see if the owners exist by name, if so we return from the func and free the names
    if (!findOwnerByName(firstname) || !findOwnerByName(secondname)) {
        printf("One or both owners not found.\n");
        free(firstname);
        free(secondname);
        return;
    }
    // Create the owners by comparing them to the existing owners and merge them, after that freeing the names
    OwnerNode *firstOwner = findOwnerByName(firstname);
    OwnerNode *secondOwner = findOwnerByName(secondname);
    /********************************************************************************************************
    check to see if both pokedex are empty because the run file does that but the instructions don't tell us
    good job!!
    *********************************************************************************************************/
    if (firstOwner->pokedexRoot == NULL && secondOwner->pokedexRoot == NULL) {
        printf("Both Pokedexes empty. Nothing to merge.\n");
        return;
    }
    printf("Merging %s and %s...\n", firstname, secondname);
    mergePokedexBFS(firstOwner, secondOwner);
    removeOwnerFromCircularList(secondOwner);
    printf("Merge completed.\n");
    printf("Owner '%s' has been removed after merging.\n", secondname);
    free(firstname);
    free(secondname);
}

// --------------------------------------------------------------
// Sort owners funcs
// --------------------------------------------------------------
int getOwnerCount(OwnerNode *owner) {
    if (owner == NULL) {
        return 0;
    }
    OwnerNode *current = owner;
    int count = 1;
    while (current->next != ownerHead) {
        count++;
        current = current->next;
    }
    return count;
}

void sortOwners() {
    // 0 or 1 owners, cant merge
    if (ownerHead == NULL || ownerHead->next == ownerHead) {
        printf("0 or 1 owners only => no need to sort.\n");
        return;
    }
    // get the owner count and create an array of owner pointer with the size of the ownercount
    int ownerCount = getOwnerCount(ownerHead);
    OwnerNode **ownersArr = malloc(ownerCount * sizeof(OwnerNode *));
    if (ownersArr == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }
    // insert the nodes into the arr
    OwnerNode *current = ownerHead;
    for (int i = 0; i < ownerCount; i++) {
        ownersArr[i] = current;
        current = current->next;
    }
    // sort them alphabetically using bubble sort
    for (int i = 0; i < ownerCount; i++) {
        for (int j = 0; j < ownerCount - i - 1; j++) {
            if (strcmp(ownersArr[j]->ownerName, ownersArr[j + 1]->ownerName) > 0) {
                swapOwnerData(ownersArr[j], ownersArr[j + 1]);
            }
        }
    }
    /* rearrange the owner list with the new order,
    giving ownerHead the value null so that linkOwnerInCircularList will work correctly, after that freeing the array*/
    ownerHead = NULL;
    for (int i = 0; i < ownerCount; i++) {
        linkOwnerInCircularList(ownersArr[i]);
    }
    free(ownersArr);
    printf("Owners sorted by name.\n");
}

// swaping date between two nodes by creating a temp node and swapping the data
void swapOwnerData(OwnerNode *a, OwnerNode *b) {
    char *tempName = a->ownerName;
    PokemonNode *tempTree = a->pokedexRoot;
    a->ownerName = b->ownerName;
    a->pokedexRoot = b->pokedexRoot;
    b->ownerName = tempName;
    b->pokedexRoot = tempTree;
}

// --------------------------------------------------------------
// Print Owners in a direction X times funcs
// --------------------------------------------------------------
void printOwnersCircular() {
    // check if there are no owners
    if (ownerHead == NULL) {
        printf("No owners.\n");
        return;
    }
    // get the direction from the user
    printf("Enter direction (F or B): ");
    // reading the string safely from the user and then "casting" it to be a single char for later use
    char *tempStr = getDynamicInput();
    char dir = tempStr[0];
    // get from the user number of prints
    int reptitions = readIntSafe("How many prints? ");
    OwnerNode *temp = ownerHead;
    // Print with each matching logic assuming the input is valid, each time freeing the tempStr
    if (dir == 'b' || dir == 'B') {
        for (int i = 1; i <= reptitions; i++) {
            printf("[%d] %s\n", i, temp->ownerName);
            temp = temp->prev;
        }
        free(tempStr);
        return;
    }
    if (dir == 'f' || dir == 'F') {
        for (int i = 1; i <= reptitions; i++) {
            printf("[%d] %s\n", i, temp->ownerName);
            temp = temp->next;
        }
        free(tempStr);
        return;
    }
    free(tempStr);
}

// --------------------------------------------------------------
// Main Menu funcs
// --------------------------------------------------------------
void mainMenu() {
    int choice;
    do {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice) {
            case 1:
                openPokedexMenu();
                break;
            case 2:
                enterExistingPokedexMenu();
                break;
            case 3:
                deletePokedex();
                break;
            case 4:
                mergePokedexMenu();
                break;
            case 5:
                sortOwners();
                break;
            case 6:
                printOwnersCircular();
                break;
            case 7:
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid.\n");
        }
    } while (choice != 7);
}

OwnerNode *findOwnerByName(char *name) {
    // check if the owner list is empty
    if (ownerHead == NULL) {
        return NULL;
    }
    //check if the owners name already exist by strcmp with all the names in the list
    OwnerNode *temp = ownerHead;
    do {
        if (!strcmp(temp->ownerName, name)) {
            return temp;
        }
        temp = temp->next;
    } while (temp != ownerHead);
    // if no owners was found
    return NULL;
}

void openPokedexMenu() {
    // initializing the pokemonode of the starter pokemon that the user will choose
    PokemonNode *starter = NULL;
    printf("Your name: ");
    char *ownerName = getDynamicInput();
    // use findownerbyname to see if the owner name already exist
    if (findOwnerByName(ownerName)) {
        printf("Owner '%s' already exists. Not creating a new Pokedex. \n", ownerName);
        free(ownerName);
        return;
    }
    // switch case for choosing a starter
    printf("Choose Starter: \n1. Bulbasaur\n2. Charmander\n3. Squirtle\n");
    int choice = readIntSafe("Your choice: ");
    switch (choice) {
        case 1: {
            starter = createPokemonNode(&pokedex[0]);
            break;
        }
        case 2: {
            starter = createPokemonNode(&pokedex[3]);
            break;
        }
        case 3: {
            starter = createPokemonNode(&pokedex[6]);
            break;
        }
        default:
            printf("Invalid choice.\n");
            free(ownerName);
            return;
    }
    // create a node for the new owner with the starter and the name he chose and then link the node in the list
    OwnerNode *newOwner = createOwner(ownerName, starter);
    linkOwnerInCircularList(newOwner);
}

//create owner allocates memory for the new owner node and initializes his values as requested
OwnerNode *createOwner(char *ownerName, PokemonNode *starter) {
    OwnerNode *newOwner = (OwnerNode *) malloc(sizeof(OwnerNode));
    if (!newOwner) {
        printf("Memory allocation error.\n");
        exit(1);
    }
    newOwner->ownerName = ownerName;
    newOwner->pokedexRoot = starter;
    newOwner->prev = NULL;
    newOwner->next = NULL;
    printf("New Pokedex created for %s with starter %s.\n", ownerName, starter->data->name);
    return newOwner;
}

void linkOwnerInCircularList(OwnerNode *newOwner) {
    // If the list is empty then newOwner will be the ownerHead
    if (ownerHead == NULL) {
        ownerHead = newOwner;
        ownerHead->next = newOwner;
        ownerHead->prev = newOwner;
        return;
    }
    //If not then we need the new owner to be the last node
    OwnerNode *last = ownerHead->prev;
    last->next = newOwner;
    newOwner->prev = last;
    newOwner->next = ownerHead;
    ownerHead->prev = newOwner;
}

//create pokemone node allocates memory for the new pokemon node and initializes his values as requested
PokemonNode *createPokemonNode(PokemonData *data) {
    PokemonNode *newNode = malloc(sizeof(PokemonNode));
    if (!newNode) {
        printf("Memory allocation error.\n");
        exit(1);
    }
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Prints the list of the owners with a matching number for each one, stopping when reaching the owner head again
void printOwnersList() {
    if (!ownerHead) {
        return;
    }
    OwnerNode *temp = ownerHead;
    int counter = 1;
    do {
        printf("%d. %s\n", counter, temp->ownerName);
        temp = temp->next;
        counter++;
    } while (temp != ownerHead);
}

void freeAllOwners() {
    // Check if the list is empty
    if (ownerHead == NULL) {
        return;
    }
    OwnerNode *temp = ownerHead;
    do {
        OwnerNode *tempNext = temp->next;
        freeOwnerNode(temp);
        temp = tempNext;
    } while (temp != ownerHead);
    ownerHead = NULL;
}

int main() {
    mainMenu();
    freeAllOwners();
    return 0;
}
