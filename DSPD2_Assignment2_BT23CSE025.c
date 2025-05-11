#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define M 4 // Maximum degree of the B-tree
#define MAX_FAMILY_MEMBERS 4
#define NAME_LEN 100
#define MAX_EXPENSES_PER_USER 100
#define MAX_CATEGORIES 5

typedef enum {
    Rent = 0,
    Utility,
    Grocery,
    Stationary,
    Leisure
} ExpenseCategory;

const char* category_names[MAX_CATEGORIES] = {
    "Rent", "Utility", "Grocery", "Stationary", "Leisure"
};

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct ExpenseNode ExpenseNode;
typedef struct UserNode UserNode;
typedef struct FamilyNode FamilyNode;

struct ExpenseNode {
    int expense_id;
    int user_id;
    float amount;
    ExpenseCategory category;
    Date date;
    ExpenseNode* next; //for chaining expenses by user
};

struct UserNode {
    int user_id;
    char user_name[NAME_LEN];
    float income;
    FamilyNode* family;
    ExpenseNode* expenses_head;
    int expense_count;
    float total_expense;
    float category_expenses[MAX_CATEGORIES];
};

struct FamilyNode {
    int family_id;
    char family_name[NAME_LEN];
    UserNode* members[MAX_FAMILY_MEMBERS];
    int member_count;
    float total_income;
    float total_expense;
    float category_expenses[MAX_CATEGORIES];
};

//B-tree node structures
typedef struct BTreeNodeUser {
    int num_keys;
    UserNode* keys[M-1];
    struct BTreeNodeUser* children[M];
    bool is_leaf;
} BTreeNodeUser;

typedef struct BTreeNodeFamily {
    int num_keys;
    FamilyNode* keys[M-1];
    struct BTreeNodeFamily* children[M];
    bool is_leaf;
} BTreeNodeFamily;

typedef struct BTreeNodeExpense {
    int num_keys;
    ExpenseNode* keys[M-1];
    struct BTreeNodeExpense* children[M];
    bool is_leaf;
} BTreeNodeExpense;

BTreeNodeUser* user_root = NULL;
BTreeNodeFamily* family_root = NULL;
BTreeNodeExpense* expense_root = NULL;

BTreeNodeUser* createUserNode(bool is_leaf);
BTreeNodeFamily* createFamilyNode(bool is_leaf);
BTreeNodeExpense* createExpenseNode(bool is_leaf);

void insertUser(BTreeNodeUser** root, UserNode* user);
void insertUserNonFull(BTreeNodeUser* node, UserNode* user);
void splitUserChild(BTreeNodeUser* parent, int idx);
void insertFamily(BTreeNodeFamily** root, FamilyNode* family);
void insertExpense(BTreeNodeExpense** root, ExpenseNode* expense);

UserNode* searchUser(BTreeNodeUser* root, int user_id);
FamilyNode* searchFamily(BTreeNodeFamily* root, int family_id);
ExpenseNode* searchExpenseForUser(UserNode* user, int expense_id);
ExpenseNode* searchExpense(BTreeNodeExpense* root, int user_id, int expense_id);

UserNode* addUser(int user_id, const char* name, float income);
FamilyNode* createFamily(int family_id, const char* family_name);
bool joinFamily(int user_id, int family_id);
ExpenseNode* addExpense(int user_id, int expense_id, float amount, ExpenseCategory category, Date date);
bool removeUser(int user_id);
bool removeFamily(int family_id);
bool removeExpense(int expense_id);

void getTotalExpense(int family_id);
void getCategoricalExpense(int family_id, ExpenseCategory category);
void getHighestExpenseDay(int family_id);
void getIndividualExpense(int user_id);
void getExpensesInPeriod(BTreeNodeExpense* root, Date start, Date end);
void getExpensesInRange(int user_id, int start_id, int end_id);

void updateFamilyTotals(FamilyNode* family);
int dateCompare(Date d1, Date d2);
void sortExpensesByAmount(ExpenseNode** expenses, int count);
void sortExpensesByCategory(ExpenseNode** expenses, int count);
void freeUser(UserNode* user);
void freeFamily(FamilyNode* family);
void freeExpense(ExpenseNode* expense);

void printUser(UserNode* user);
void printFamily(FamilyNode* family);
void printExpense(ExpenseNode* expense);
void traverseAndPrintUsers(BTreeNodeUser* root);
void traverseAndPrintFamilies(BTreeNodeFamily* root);
void traverseAndPrintExpenses(BTreeNodeExpense* root);
void printAllUsers();
void printAllFamilies();
void printAllExpenses();

// Individual B-tree deletion functions
int find_key_index_individual(BTreeNodeUser* node, int user_id);
void removeFromLeafIndividual(BTreeNodeUser* node, int idx);
UserNode* getIndividualPredecessor(BTreeNodeUser* node, int idx);
UserNode* getIndividualSuccessor(BTreeNodeUser* node, int idx);
void fillIndividualChild(BTreeNodeUser* node, int idx);
void borrowFromLeftIndividual(BTreeNodeUser* parent, int idx);
void borrowFromRightIndividual(BTreeNodeUser* parent, int idx);
void mergeIndividualNodes(BTreeNodeUser* node, int idx);
void deleteFromIndividualSubtree(BTreeNodeUser* node, int user_id);
bool deleteIndividual(BTreeNodeUser** root, int user_id);

// Family B-tree deletion functions
int find_key_index_family(BTreeNodeFamily* node, int family_id);
void removeFromLeafFamily(BTreeNodeFamily* node, int idx);
FamilyNode* getFamilyPredecessor(BTreeNodeFamily* node, int idx);
FamilyNode* getFamilySuccessor(BTreeNodeFamily* node, int idx);
void fillFamilyChild(BTreeNodeFamily* node, int idx);
void borrowFromLeftFamily(BTreeNodeFamily* parent, int idx);
void borrowFromRightFamily(BTreeNodeFamily* parent, int idx);
void mergeFamilyNodes(BTreeNodeFamily* node, int idx);
void deleteFromFamilySubtree(BTreeNodeFamily* node, int family_id);
bool deleteFamily(BTreeNodeFamily** root, int family_id);

// Expense B-tree deletion functions
int find_key_index_expense(BTreeNodeExpense* node, int user_id, int expense_id);
void removeFromLeafExpense(BTreeNodeExpense* node, int idx);
ExpenseNode* getExpensePredecessor(BTreeNodeExpense* node, int idx);
ExpenseNode* getExpenseSuccessor(BTreeNodeExpense* node, int idx);
void fillExpenseChild(BTreeNodeExpense* node, int idx);
void borrowFromLeftExpense(BTreeNodeExpense* parent, int idx);
void borrowFromRightExpense(BTreeNodeExpense* parent, int idx);
void mergeExpenseNodes(BTreeNodeExpense* node, int idx);
void deleteFromExpenseSubtree(BTreeNodeExpense* node, int user_id, int expense_id);
bool deleteExpense(BTreeNodeExpense** root, int user_id, int expense_id);

void updateOrDeleteIndividualFamilyDetails(BTreeNodeUser** user_root,BTreeNodeFamily** family_root,BTreeNodeExpense** expense_root);
void updateOrDeleteExpense();


BTreeNodeUser* createUserNode(bool is_leaf){
    BTreeNodeUser* node = (BTreeNodeUser*)malloc(sizeof(BTreeNodeUser));
    if (!node) {
        printf("Failed to allocate memory for user node");
    }
    else{
        node->num_keys = 0;
        node->is_leaf = is_leaf;
        for (int i = 0; i < M; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

void insertUser(BTreeNodeUser** root, UserNode* user){
    if(*root == NULL){
        *root = createUserNode(true);
        (*root)->keys[0] = user;
        (*root)->num_keys = 1;
    }
    else{
        if((*root)->num_keys == M - 1){
            BTreeNodeUser* new_root = createUserNode(false);
            new_root->children[0] = *root;
            *root = new_root;
            splitUserChild(*root, 0);
        }
    
        insertUserNonFull(*root, user);
    }
    
}

void insertUserNonFull(BTreeNodeUser* node, UserNode* user) {
    int i = node->num_keys - 1;

    if(node->is_leaf){
        while (i >= 0 && user->user_id < node->keys[i]->user_id){
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = user;
        node->num_keys++;
    }
    else{
        while (i >= 0 && user->user_id < node->keys[i]->user_id){
            i--;
        }
        i++;

        if(node->children[i]->num_keys == M - 1){
            splitUserChild(node, i);
            if (user->user_id > node->keys[i]->user_id){
                i++;
            }
        }
        insertUserNonFull(node->children[i], user);
    }
}

void splitUserChild(BTreeNodeUser* parent, int idx) {
    BTreeNodeUser* child = parent->children[idx];
    BTreeNodeUser* new_child = createUserNode(child->is_leaf);
    new_child->num_keys = M / 2 - 1;

    for (int j = 0; j < M / 2 - 1; j++) {
        new_child->keys[j] = child->keys[j + M / 2];
    }

    if (!child->is_leaf) {
        for (int j = 0; j < M / 2; j++) {
            new_child->children[j] = child->children[j + M / 2];
        }
    }

    child->num_keys = M / 2 - 1;

    for (int j = parent->num_keys; j > idx; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[idx + 1] = new_child;

    for (int j = parent->num_keys - 1; j >= idx; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }
    parent->keys[idx] = child->keys[M / 2 - 1];
    parent->num_keys++;
}

UserNode* searchUser(BTreeNodeUser* root, int user_id){
    UserNode* ret_node = NULL;
    if(!root){
        ret_node =  NULL;
    }
    else{
        int i = 0;
        while (i < root->num_keys && user_id > root->keys[i]->user_id) {
            i++;
        }

        if(i < root->num_keys && user_id == root->keys[i]->user_id) {
            ret_node = root->keys[i];
        }

        else if(root->is_leaf) {
            ret_node = NULL;
        }
        else{
            ret_node = searchUser(root->children[i], user_id);
        }    
    }
    return ret_node;
    
}

int findUserKeyIndex(BTreeNodeUser* node, int user_id){
    int index = 0;
    while (index < node->num_keys && user_id > node->keys[index]->user_id){
        index++;
    }
    return index;
}

int findFamilyKeyIndex(BTreeNodeFamily *node, int family_id){
    int index = 0;
    while (index < node->num_keys && family_id > node->keys[index]->family_id){
        index++;
    }
    return index;
}

int findExpenseKeyIndex(BTreeNodeExpense *node, int expense_id){
    int index = 0;
    while (index < node->num_keys && expense_id > node->keys[index]->expense_id){
        index++;
    }
    return index;
}

// create a new family node
BTreeNodeFamily *createFamilyNode(bool is_leaf){
    BTreeNodeFamily *newNode = (BTreeNodeFamily *)malloc(sizeof(BTreeNodeFamily));
    if (newNode == NULL) {
        printf("Memory allocation failed");
    }
    else{
        newNode->num_keys = 0;
        newNode->is_leaf = is_leaf;
        for (int i = 0; i < M; i++) {
            newNode->children[i] = NULL;
        }
    }
    return newNode;
}

void splitFamilyChild(BTreeNodeFamily *parent, int index){
    BTreeNodeFamily *child = parent->children[index];
    BTreeNodeFamily *newNode = createFamilyNode(child->is_leaf);
    
    newNode->num_keys = M/2 - 1;
    
    //move keys to the new node
    for(int i = 0; i < M/2 - 1; i++){
        newNode->keys[i] = child->keys[i + M/2];
    }
    
    if(!child->is_leaf){
        //move children to the new node
        for(int i = 0; i < M/2; i++){
            newNode->children[i] = child->children[i + M/2];
        }
    }
    
    child->num_keys = M/2 - 1;
    
    //shift parent's children to make space
    for(int i = parent->num_keys; i > index; i--){
        parent->children[i + 1] = parent->children[i];
    }
    
    parent->children[index + 1] = newNode;
    
    //shift parent's keys to insert the middle key from the child
    for(int i = parent->num_keys - 1; i >= index; i--){
        parent->keys[i + 1] = parent->keys[i];
    }
    
    //move the middle key from child to parent
    parent->keys[index] = child->keys[M/2 - 1];
    parent->num_keys++;
}

//insert a family into a non-full node
void insertFamilyNonFull(BTreeNodeFamily *node, FamilyNode* family) {
    int i = node->num_keys - 1;
    
    if(node->is_leaf){
        //insert key into the sorted order
        while(i >= 0 && node->keys[i]->family_id > family->family_id){
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = family;
        node->num_keys++;
    }
    else{
        // Find the child to insert the key
        while(i >= 0 && node->keys[i]->family_id > family->family_id){
            i--;
        }
        i++;
        
        if(node->children[i]->num_keys == M - 1){
            // Split child if it's full
            splitFamilyChild(node, i);
            
            // Determine which of the two children is the new one
            if(node->keys[i]->family_id < family->family_id){
                i++;
            }
        }
        insertFamilyNonFull(node->children[i], family);
    }
}

//insert a family into the B-tree
void insertFamily(BTreeNodeFamily **root, FamilyNode* family){
    BTreeNodeFamily *node = *root;

    if(node == NULL){
        *root = createFamilyNode(true);
        (*root)->keys[0] = family;
        (*root)->num_keys = 1;
    }
    else{
        if(node->num_keys == M - 1){
            // split the root if it's full
            BTreeNodeFamily *new_root = createFamilyNode(false);
            new_root->children[0] = node;
            splitFamilyChild(new_root, 0);
            *root = new_root;
            
            //determine which child to insert into
            int i = 0;
            if (new_root->keys[0]->family_id < family->family_id) {
                i++;
            }
            insertFamilyNonFull(new_root->children[i], family);
        }
        else{
            insertFamilyNonFull(node, family);
        }
    }
}

FamilyNode *searchFamily(BTreeNodeFamily *root, int family_id){
    FamilyNode* ret_node = NULL;
    if (root == NULL) {
        ret_node = NULL;
    }
    else{
        int index = findFamilyKeyIndex(root, family_id);
    
        if(index < root->num_keys && root->keys[index]->family_id == family_id) {
            ret_node = root->keys[index];
        }
        
        else if(root->is_leaf){
            ret_node = NULL;
        }
        else{
            ret_node = searchFamily(root->children[index], family_id);
        }
    }
    return ret_node;
    
}

//Create expense node
BTreeNodeExpense *createExpenseNode(bool is_leaf){
    BTreeNodeExpense *newNode = (BTreeNodeExpense *)malloc(sizeof(BTreeNodeExpense));
    if (newNode == NULL){
        printf("Memory allocation failed");
    }
    else{
        newNode->num_keys = 0;
        newNode->is_leaf = is_leaf;
        for (int i = 0; i < M; i++) {
            newNode->children[i] = NULL;
        }
    }
    return newNode;
}

void splitExpenseChild(BTreeNodeExpense *parent, int index){
    BTreeNodeExpense *child = parent->children[index];
    BTreeNodeExpense *newNode = createExpenseNode(child->is_leaf);
    
    newNode->num_keys = M/2 - 1;
    
    // Move keys to the new node
    for (int i = 0; i < M/2 - 1; i++){
        newNode->keys[i] = child->keys[i + M/2];
    }
    
    if(!child->is_leaf){
        // move children to the new node if not a leaf
        for(int i = 0; i < M/2; i++){
            newNode->children[i] = child->children[i + M/2];
        }
    }
    
    child->num_keys = M/2 - 1;
    
    //shift parent's children to make space for the new node
    for (int i = parent->num_keys; i > index; i--){
        parent->children[i + 1] = parent->children[i];
    }
    
    parent->children[index + 1] = newNode;
    
    //shift parent's keys to insert the middle key from the child
    for (int i = parent->num_keys - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }
    
    //move the middle key from child to parent
    parent->keys[index] = child->keys[M/2 - 1];
    parent->num_keys++;
}

void insertExpenseNonFull(BTreeNodeExpense* node, ExpenseNode* expense){
    int i = node->num_keys - 1;
    
    if(node->is_leaf){
        while(i >= 0 && 
              (node->keys[i]->user_id > expense->user_id ||
               (node->keys[i]->user_id == expense->user_id && 
                node->keys[i]->expense_id > expense->expense_id))){
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = expense;
        node->num_keys++;
    }
    else{
        //find child to insert into
        while(i >= 0 && 
              (node->keys[i]->user_id > expense->user_id ||
               (node->keys[i]->user_id == expense->user_id && 
                node->keys[i]->expense_id > expense->expense_id))){
            i--;
        }
        i++;
        
        if(node->children[i]->num_keys == M - 1){
            splitExpenseChild(node, i);
            if (node->keys[i]->user_id < expense->user_id ||
                (node->keys[i]->user_id == expense->user_id && 
                 node->keys[i]->expense_id < expense->expense_id)){
                i++;
            }
        }
        insertExpenseNonFull(node->children[i], expense);
    }
}

// Insert an expense into the B-tree
void insertExpense(BTreeNodeExpense **root, ExpenseNode* expense) {
    BTreeNodeExpense *node = *root;

    if(node == NULL){
        *root = createExpenseNode(true);
        (*root)->keys[0] = expense;
        (*root)->num_keys = 1;
    }
    else{
        if(node->num_keys == M - 1){
            //split the root if it's full
            BTreeNodeExpense *new_root = createExpenseNode(false);
            new_root->children[0] = node;
            splitExpenseChild(new_root, 0);
            *root = new_root;
            
            //determine which child to insert into
            int i = 0;
            if(new_root->keys[0]->expense_id < expense->expense_id){
                i++;
            }
            insertExpenseNonFull(new_root->children[i], expense);
        }
        else{
            insertExpenseNonFull(node, expense);
        }
    }
}

ExpenseNode* searchExpenseForUser(UserNode* user, int expense_id) {
    if (!user){
        return NULL;
    }  
    ExpenseNode* current = user->expenses_head;
    while (current) {
        if (current->expense_id == expense_id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Search for an expense in the B-tree
ExpenseNode* searchExpense(BTreeNodeExpense* root, int user_id, int expense_id){
    if(root == NULL){
        return NULL;
    }
    
    int i = 0;
    while(i < root->num_keys && 
          (root->keys[i]->user_id < user_id || 
           (root->keys[i]->user_id == user_id && 
            root->keys[i]->expense_id < expense_id))){
        i++;
    }
    
    if(i < root->num_keys && 
        root->keys[i]->user_id == user_id && 
        root->keys[i]->expense_id == expense_id){
        return root->keys[i];
    }
    
    return root->is_leaf ? NULL : searchExpense(root->children[i], user_id, expense_id);
}


UserNode* addUser(int user_id, const char* name, float income){
    UserNode* ret_node;
    if(searchUser(user_root, user_id)){
        ret_node = NULL; //user already exists
    }
    else{
        UserNode* new_user = (UserNode*)malloc(sizeof(UserNode));
        new_user->user_id = user_id;
        strncpy(new_user->user_name, name, NAME_LEN);
        new_user->income = income;
        new_user->family = NULL;
        new_user->expenses_head = NULL;
        new_user->expense_count = 0;
        new_user->total_expense = 0.0f;
        memset(new_user->category_expenses, 0, sizeof(new_user->category_expenses));

        insertUser(&user_root, new_user);
        ret_node = new_user;
    }
    
    return ret_node;
}

FamilyNode* createFamily(int family_id, const char* family_name){
    FamilyNode* ret_node;
    if(searchFamily(family_root, family_id)){
        ret_node = NULL; //Family already exists
    }
    else{
        FamilyNode* new_family = (FamilyNode*)malloc(sizeof(FamilyNode));
        new_family->family_id = family_id;
        strncpy(new_family->family_name, family_name, NAME_LEN);
        new_family->member_count = 0;
        new_family->total_income = 0.0f;
        new_family->total_expense = 0.0f;
        memset(new_family->category_expenses, 0, sizeof(new_family->category_expenses));
        for (int i = 0; i < MAX_FAMILY_MEMBERS; i++) {
            new_family->members[i] = NULL;
        }

        insertFamily(&family_root, new_family);
        ret_node = new_family;
    }
    
    return ret_node;
}

bool joinFamily(int user_id, int family_id){
    UserNode* user = searchUser(user_root, user_id);
    FamilyNode* family = searchFamily(family_root, family_id);
    bool done;

    if(!user || !family){
        printf("Error: User %d or Family %d not found\n", user_id, family_id);
        done = false;
    }
    else if(user->family){
        printf("Error: User %d already belongs to family %d\n", user_id, user->family->family_id);
        done = false;
    }
    else if(family->member_count >= MAX_FAMILY_MEMBERS) {
        printf("Error: Family %d is full (max %d members)\n", family_id, MAX_FAMILY_MEMBERS);
        done = false;
    }
    else{
        //ddd user to family
        family->members[family->member_count++] = user;
        family->total_income += user->income;
        family->total_expense += user->total_expense;
        
        //update category expenses
        for (int i = 0; i < MAX_CATEGORIES; i++) {
            family->category_expenses[i] += user->category_expenses[i];
        }

        //set user's family
        user->family = family;
        done = true;
    }
    return done;
}

ExpenseNode* addExpense(int user_id, int expense_id, float amount, ExpenseCategory category, Date date) {
    UserNode* user = searchUser(user_root, user_id);
    if (!user) {
        printf("Error: User %d not found\n", user_id);
        return NULL;
    }

    // Check if this user already has an expense with this ID
    if (searchExpenseForUser(user, expense_id)) {
        printf("Error: User %d already has expense with ID %d\n", user_id, expense_id);
        return NULL;
    }

    ExpenseNode* new_expense = (ExpenseNode*)malloc(sizeof(ExpenseNode));
    if (!new_expense) {
        perror("Error allocating memory for expense");
        return NULL;
    }

    new_expense->expense_id = expense_id;
    new_expense->user_id = user_id;
    new_expense->amount = amount;
    new_expense->category = category;
    new_expense->date = date;
    new_expense->next = NULL;

    // Add to user's expense list
    if(!user->expenses_head){
        user->expenses_head = new_expense;
    }
    else{
        ExpenseNode* current = user->expenses_head;
        while (current->next) {
            current = current->next;
        }
        current->next = new_expense;
    }

    // Update user totals
    user->expense_count++;
    user->total_expense += amount;
    user->category_expenses[category] += amount;

    // Update family totals if user is in a family
    if (user->family) {
        user->family->total_expense += amount;
        user->family->category_expenses[category] += amount;
    }

    // Insert into expense B-tree
    insertExpense(&expense_root, new_expense);
    
    return new_expense;
}

void getTotalExpense(int family_id){
    FamilyNode* family = searchFamily(family_root, family_id);
    if(!family) {
        printf("Family not found\n");
        return;
    }

    printf("Family: %s (ID: %d)\n", family->family_name, family->family_id);
    printf("Total Income: %.2f\n", family->total_income);
    printf("Total Expenses: %.2f\n", family->total_expense);

    float balance = family->total_income - family->total_expense;
    if(balance < 0){
        printf("Warning: Expenses exceed income by %.2f\n", -balance);
    }
    else{
        printf("Remaining balance: %.2f\n", balance);
    }
}

void getCategoricalExpense(int family_id, ExpenseCategory category){
    FamilyNode* family = searchFamily(family_root, family_id);
    if (!family) {
        printf("Family not found\n");
        return;
    }

    printf("Category: %s\n", category_names[category]);
    printf("Total family expense: %.2f\n", family->category_expenses[category]);

    // Collect individual contributions
    typedef struct {
        UserNode* user;
        float amount;
    } Contribution;

    Contribution contributions[MAX_FAMILY_MEMBERS];
    int count = 0;

    for(int i = 0; i < family->member_count; i++){
        contributions[count].user = family->members[i];
        contributions[count].amount = family->members[i]->category_expenses[category];
        count++;
    }

    // Sort contributions by amount (descending)
    for(int i = 0; i < count - 1; i++){
        for(int j = 0; j < count - i - 1; j++){
            if(contributions[j].amount < contributions[j + 1].amount){
                Contribution temp = contributions[j];
                contributions[j] = contributions[j + 1];
                contributions[j + 1] = temp;
            }
        }
    }

    // Print sorted contributions
    printf("Individual contributions:\n");
    for (int i = 0; i < count; i++){
        printf("%s (ID: %d): %.2f\n",
               contributions[i].user->user_name,
               contributions[i].user->user_id,
               contributions[i].amount);
    }
}

void getHighestExpenseDay(int family_id){
    FamilyNode* family = searchFamily(family_root, family_id);
    if(!family){
        printf("Family not found\n");
    }
    else{
        Date max_date = {0};
        float max_amount = 0.0f;

        for(int i = 0; i < family->member_count; i++){
            UserNode* user = family->members[i];
            ExpenseNode* expense = user->expenses_head;
            while(expense){
                if(expense->amount > max_amount){
                    max_amount = expense->amount;
                    max_date = expense->date;
                }
                expense = expense->next;
            }
        }

        if(max_amount > 0){
            printf("Highest expense day: %d/%d/%d (Amount: %.2f)\n",
                max_date.day, max_date.month, max_date.year, max_amount);
        }
        else{
            printf("No expenses found for this family\n");
        }
    }
    
}

void getIndividualExpense(int user_id){
    UserNode* user = searchUser(user_root, user_id);
    if(!user){
        printf("User not found\n");
        return;
    }
    
    printf("User: %s (ID: %d)\n", user->user_name, user->user_id);
    printf("Total expenses: %.2f\n", user->total_expense);

    printf("Expenses by category:\n");
    for (int i = 0; i < MAX_CATEGORIES; i++) {
        if (user->category_expenses[i] > 0) {
            printf("%s: %.2f\n", category_names[i], user->category_expenses[i]);
        }
    }

    //print all expenses sorted by amount
    printf("All expenses:\n");
    ExpenseNode* expenses[MAX_EXPENSES_PER_USER];
    int count = 0;
    
    ExpenseNode* current = user->expenses_head;
    while (current && count < MAX_EXPENSES_PER_USER) {
        expenses[count++] = current;
        current = current->next;
    }

    //sort expenses by amount descending
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (expenses[j]->amount < expenses[j + 1]->amount) {
                ExpenseNode* temp = expenses[j];
                expenses[j] = expenses[j + 1];
                expenses[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < count; i++) {
        printf("ID: %d, Amount: %.2f, Category: %s, Date: %d/%d/%d\n",
               expenses[i]->expense_id,
               expenses[i]->amount,
               category_names[expenses[i]->category],
               expenses[i]->date.day,
               expenses[i]->date.month,
               expenses[i]->date.year);
    }
}

int dateCompare(Date d1, Date d2){
    int ret_val;
    if(d1.year != d2.year){
        ret_val = d1.year - d2.year;
    } 
    else if(d1.month != d2.month){
        ret_val = d1.month - d2.month;
    }  
    else{
       ret_val = d1.day - d2.day;
    } 

    return ret_val;
}

void getExpensesInPeriod(BTreeNodeExpense* root, Date start, Date end) {
    if(root == NULL){
        printf("Expense Not Found!!\n");
    }
    else{
        for(int i = 0; i < root->num_keys; i++){
            if(!root->is_leaf){
                getExpensesInPeriod(root->children[i], start, end);
            }
    
            ExpenseNode* expense = root->keys[i];
            if(expense != NULL){
                int cmp_start = dateCompare(expense->date, start);
                int cmp_end = dateCompare(expense->date, end);
                
                if(cmp_start >= 0 && cmp_end <= 0){
                    printExpense(expense);
                }
            }
        }
    
        if(!root->is_leaf){
            getExpensesInPeriod(root->children[root->num_keys], start, end);
        }
    }
}

void getExpensesInRange(int user_id, int start_id, int end_id) {
    UserNode* user = searchUser(user_root, user_id);
    if(!user){
        printf("User not found\n");
    }
    else{
        printf("Expenses for user %s (ID: %d) between expense IDs %d and %d:\n",
           user->user_name, user->user_id, start_id, end_id);

        ExpenseNode* current = user->expenses_head;
        while (current) {
            if (current->expense_id >= start_id && current->expense_id <= end_id) {
                printf("ID: %d, Amount: %.2f, Category: %s, Date: %d/%d/%d\n",
                    current->expense_id,
                    current->amount,
                    category_names[current->category],
                    current->date.day,
                    current->date.month,
                    current->date.year);
            }
            current = current->next;
        }
    }
    
}

int find_key_index_individual(BTreeNodeUser* node, int user_id) {
    int idx = 0;
    while (idx < node->num_keys && node->keys[idx]->user_id < user_id) {
        idx++;
    }
    return idx;
}

void removeFromLeafIndividual(BTreeNodeUser* node, int idx) {
    //update any related data
    UserNode* user = node->keys[idx];
    if (user->family) {
        // Remove user from family if needed
        bool found = false;
        for(int i = 0; i < user->family->member_count && !found; i++){
            if (user->family->members[i] == user) {
                // Shift remaining members
                for(int j = i; j < user->family->member_count-1; j++){
                    user->family->members[j] = user->family->members[j+1];
                }
                user->family->member_count--;
                user->family->total_income -= user->income;
                found = true;
            }
        }
    }
    
    free(node->keys[idx]);
    
    // Shift keys
    for(int i = idx+1; i < node->num_keys; i++){
        node->keys[i-1] = node->keys[i];
    }
    node->num_keys--;
}

UserNode* getIndividualPredecessor(BTreeNodeUser* node, int idx){
    BTreeNodeUser* curr = node->children[idx];
    while(!curr->is_leaf)
        curr = curr->children[curr->num_keys];
    return curr->keys[curr->num_keys-1];
}

UserNode* getIndividualSuccessor(BTreeNodeUser* node, int idx){
    BTreeNodeUser* curr = node->children[idx+1];
    while(!curr->is_leaf)
        curr = curr->children[0];
    return curr->keys[0];
}

void fillIndividualChild(BTreeNodeUser* node, int idx){
    if(idx != 0 && node->children[idx-1]->num_keys >= M/2)
        borrowFromLeftIndividual(node, idx);
    else if(idx != node->num_keys && node->children[idx+1]->num_keys >= M/2)
        borrowFromRightIndividual(node, idx);
    else{
        if(idx != node->num_keys)
            mergeIndividualNodes(node, idx);
        else
            mergeIndividualNodes(node, idx-1);
    }
}

void borrowFromLeftIndividual(BTreeNodeUser* parent, int idx){
    BTreeNodeUser* child = parent->children[idx];
    BTreeNodeUser* sibling = parent->children[idx-1];

    // Shift child's keys right
    for(int i = child->num_keys-1; i >= 0; i--){
        child->keys[i+1] = child->keys[i];
    }
    if(!child->is_leaf){
        for (int i = child->num_keys; i >= 0; i--)
            child->children[i+1] = child->children[i];
    }

    // Move key from parent to child
    child->keys[0] = parent->keys[idx-1];
    if(!child->is_leaf)
        child->children[0] = sibling->children[sibling->num_keys];

    // Move key from sibling to parent
    parent->keys[idx-1] = sibling->keys[sibling->num_keys-1];

    child->num_keys++;
    sibling->num_keys--;
}

void borrowFromRightIndividual(BTreeNodeUser* parent, int idx){
    BTreeNodeUser* child = parent->children[idx];
    BTreeNodeUser* sibling = parent->children[idx+1];

    // Move key from parent to child
    child->keys[child->num_keys] = parent->keys[idx];
    if(!child->is_leaf)
        child->children[child->num_keys+1] = sibling->children[0];

    // Move key from sibling to parent
    parent->keys[idx] = sibling->keys[0];

    // Shift sibling's keys left
    for(int i = 1; i < sibling->num_keys; i++){
        sibling->keys[i-1] = sibling->keys[i];
    }
    if(!sibling->is_leaf){
        for(int i = 1; i <= sibling->num_keys; i++)
            sibling->children[i-1] = sibling->children[i];
    }

    child->num_keys++;
    sibling->num_keys--;
}

void mergeIndividualNodes(BTreeNodeUser* node, int idx){
    BTreeNodeUser* child = node->children[idx];
    BTreeNodeUser* sibling = node->children[idx+1];

    // Move key from parent to child
    child->keys[M/2-1] = node->keys[idx];

    // Copy keys from sibling to child
    for(int i = 0; i < sibling->num_keys; i++){
        child->keys[i+M/2] = sibling->keys[i];
    }
    if(!child->is_leaf){
        for(int i = 0; i <= sibling->num_keys; i++)
            child->children[i+M/2] = sibling->children[i];
    }

    // Shift parent's keys and children
    for(int i = idx+1; i < node->num_keys; i++){
        node->keys[i-1] = node->keys[i];
    }
    for(int i = idx+2; i <= node->num_keys; i++)
        node->children[i-1] = node->children[i];

    child->num_keys += sibling->num_keys + 1;
    node->num_keys--;

    free(sibling);
}

void deleteFromIndividualSubtree(BTreeNodeUser* node, int user_id){
    int idx = find_key_index_individual(node, user_id);

    if (idx < node->num_keys && node->keys[idx]->user_id == user_id) {
        if (node->is_leaf) {
            removeFromLeafIndividual(node, idx);
        }
        else{
            if (node->children[idx]->num_keys >= M/2) {
                UserNode* pred = getIndividualPredecessor(node, idx);
                node->keys[idx] = pred;
                deleteFromIndividualSubtree(node->children[idx], pred->user_id);
            }
            else if(node->children[idx+1]->num_keys >= M/2){
                UserNode* succ = getIndividualSuccessor(node, idx);
                node->keys[idx] = succ;
                deleteFromIndividualSubtree(node->children[idx+1], succ->user_id);
            }
            else{
                mergeIndividualNodes(node, idx);
                deleteFromIndividualSubtree(node->children[idx], user_id);
            }
        }
    }
    else{
        if(node->is_leaf){
            printf("User ID %d not found.\n", user_id);
            return;
        }

        bool flag = (idx == node->num_keys);
        if (node->children[idx]->num_keys < M/2)
            fillIndividualChild(node, idx);

        if (flag && idx > node->num_keys)
            deleteFromIndividualSubtree(node->children[idx-1], user_id);
        else
            deleteFromIndividualSubtree(node->children[idx], user_id);
    }
}

bool deleteIndividual(BTreeNodeUser** root, int user_id){
    if(*root == NULL){
        printf("User with ID %d not found!\n", user_id);
        return false;
    }

    deleteFromIndividualSubtree(*root, user_id);

    if((*root)->num_keys == 0){
        BTreeNodeUser* temp = *root;
        if((*root)->is_leaf)
            *root = NULL;
        else
            *root = (*root)->children[0];
        free(temp);
    }

    return true;
}

int find_key_index_family(BTreeNodeFamily* node, int key) {
    int idx = 0;
    while (idx < node->num_keys && node->keys[idx]->family_id < key) {
        idx++;
    }
    return idx;
}

void removeFromLeafFamily(BTreeNodeFamily* node, int idx) {
    // First remove all members from this family
    for (int i = 0; i < node->keys[idx]->member_count; i++) {
        if (node->keys[idx]->members[i]) {
            node->keys[idx]->members[i]->family = NULL;
        }
    }
    free(node->keys[idx]); // Free family memory
    
    // Shift keys and values
    for (int i = idx+1; i < node->num_keys; i++) {
        node->keys[i-1] = node->keys[i];
    }
    node->num_keys--;
}

FamilyNode* getFamilyPredecessor(BTreeNodeFamily* node, int idx) {
    BTreeNodeFamily* curr = node->children[idx];
    while (!curr->is_leaf)
        curr = curr->children[curr->num_keys];
    return curr->keys[curr->num_keys-1];
}

FamilyNode* getFamilySuccessor(BTreeNodeFamily* node, int idx) {
    BTreeNodeFamily* curr = node->children[idx+1];
    while (!curr->is_leaf)
        curr = curr->children[0];
    return curr->keys[0];
}

void fillFamilyChild(BTreeNodeFamily* node, int idx) {
    if (idx != 0 && node->children[idx-1]->num_keys >= M/2)
        borrowFromLeftFamily(node, idx);
    else if (idx != node->num_keys && node->children[idx+1]->num_keys >= M/2)
        borrowFromRightFamily(node, idx);
    else {
        if (idx != node->num_keys)
            mergeFamilyNodes(node, idx);
        else
            mergeFamilyNodes(node, idx-1);
    }
}

void borrowFromLeftFamily(BTreeNodeFamily* parent, int idx) {
    BTreeNodeFamily* child = parent->children[idx];
    BTreeNodeFamily* sibling = parent->children[idx-1];

    // Shift child's keys and children right
    for (int i = child->num_keys-1; i >= 0; i--) {
        child->keys[i+1] = child->keys[i];
    }
    if (!child->is_leaf) {
        for (int i = child->num_keys; i >= 0; i--)
            child->children[i+1] = child->children[i];
    }

    // Move key from parent to child
    child->keys[0] = parent->keys[idx-1];
    if (!child->is_leaf)
        child->children[0] = sibling->children[sibling->num_keys];

    // Move key from sibling to parent
    parent->keys[idx-1] = sibling->keys[sibling->num_keys-1];

    child->num_keys++;
    sibling->num_keys--;
}

void borrowFromRightFamily(BTreeNodeFamily* parent, int idx) {
    BTreeNodeFamily* child = parent->children[idx];
    BTreeNodeFamily* sibling = parent->children[idx+1];

    // Move key from parent to child
    child->keys[child->num_keys] = parent->keys[idx];
    if (!child->is_leaf)
        child->children[child->num_keys+1] = sibling->children[0];

    // Move key from sibling to parent
    parent->keys[idx] = sibling->keys[0];

    // Shift sibling's keys and children left
    for (int i = 1; i < sibling->num_keys; i++) {
        sibling->keys[i-1] = sibling->keys[i];
    }
    if (!sibling->is_leaf) {
        for (int i = 1; i <= sibling->num_keys; i++)
            sibling->children[i-1] = sibling->children[i];
    }

    child->num_keys++;
    sibling->num_keys--;
}

void mergeFamilyNodes(BTreeNodeFamily* node, int idx) {
    BTreeNodeFamily* child = node->children[idx];
    BTreeNodeFamily* sibling = node->children[idx+1];

    // Move key from parent to child
    child->keys[M/2-1] = node->keys[idx];

    // Copy keys and children from sibling to child
    for (int i = 0; i < sibling->num_keys; i++) {
        child->keys[i+M/2] = sibling->keys[i];
    }
    if (!child->is_leaf) {
        for (int i = 0; i <= sibling->num_keys; i++)
            child->children[i+M/2] = sibling->children[i];
    }

    // Shift parent's keys and children
    for (int i = idx+1; i < node->num_keys; i++) {
        node->keys[i-1] = node->keys[i];
    }
    for (int i = idx+2; i <= node->num_keys; i++)
        node->children[i-1] = node->children[i];

    child->num_keys += sibling->num_keys + 1;
    node->num_keys--;

    free(sibling);
}

void deleteFromFamilySubtree(BTreeNodeFamily* node, int family_id) {
    int idx = find_key_index_family(node, family_id);

    if (idx < node->num_keys && node->keys[idx]->family_id == family_id) {
        if (node->is_leaf) {
            removeFromLeafFamily(node, idx);
        } else {
            if (node->children[idx]->num_keys >= M/2) {
                FamilyNode* pred = getFamilyPredecessor(node, idx);
                node->keys[idx] = pred;
                deleteFromFamilySubtree(node->children[idx], pred->family_id);
            } else if (node->children[idx+1]->num_keys >= M/2) {
                FamilyNode* succ = getFamilySuccessor(node, idx);
                node->keys[idx] = succ;
                deleteFromFamilySubtree(node->children[idx+1], succ->family_id);
            } else {
                mergeFamilyNodes(node, idx);
                deleteFromFamilySubtree(node->children[idx], family_id);
            }
        }
    } else {
        if (node->is_leaf) {
            printf("Family ID %d not found.\n", family_id);
            return;
        }

        bool flag = (idx == node->num_keys);
        if (node->children[idx]->num_keys < M/2)
            fillFamilyChild(node, idx);

        if (flag && idx > node->num_keys)
            deleteFromFamilySubtree(node->children[idx-1], family_id);
        else
            deleteFromFamilySubtree(node->children[idx], family_id);
    }
}

bool deleteFamily(BTreeNodeFamily** root, int family_id) {
    if (*root == NULL) {
        printf("Family with ID %d not found!\n", family_id);
        return false;
    }

    deleteFromFamilySubtree(*root, family_id);

    if ((*root)->num_keys == 0) {
        BTreeNodeFamily* temp = *root;
        if ((*root)->is_leaf)
            *root = NULL;
        else
            *root = (*root)->children[0];
        free(temp);
    }

    return true;
}

int find_key_index_expense(BTreeNodeExpense* node, int user_id, int expense_id){
    int idx = 0;
    while(idx < node->num_keys && 
          (node->keys[idx]->user_id < user_id || 
           (node->keys[idx]->user_id == user_id && node->keys[idx]->expense_id < expense_id))){
        idx++;
    }
    return idx;
}

void removeFromLeafExpense(BTreeNodeExpense* node, int idx){
    // First update user and family totals
    UserNode* user = searchUser(user_root, node->keys[idx]->user_id);
    if(user){
        user->total_expense -= node->keys[idx]->amount;
        user->category_expenses[node->keys[idx]->category] -= node->keys[idx]->amount;
        
        if(user->family){
            user->family->total_expense -= node->keys[idx]->amount;
            user->family->category_expenses[node->keys[idx]->category] -= node->keys[idx]->amount;
        }
        
        // Remove from user's linked list
        ExpenseNode* prev = NULL;
        ExpenseNode* current = user->expenses_head;
        while (current && current != node->keys[idx]) {
            prev = current;
            current = current->next;
        }
        
        if(current){
            if(prev){
                prev->next = current->next;
            }else{
                user->expenses_head = current->next;
            }
        }
    }
    
    free(node->keys[idx]); // Free expense memory
    
    // Shift keys
    for (int i = idx+1; i < node->num_keys; i++) {
        node->keys[i-1] = node->keys[i];
    }
    node->num_keys--;
}

ExpenseNode* getExpensePredecessor(BTreeNodeExpense* node, int idx) {
    BTreeNodeExpense* curr = node->children[idx];
    while (!curr->is_leaf)
        curr = curr->children[curr->num_keys];
    return curr->keys[curr->num_keys-1];
}

ExpenseNode* getExpenseSuccessor(BTreeNodeExpense* node, int idx){
    BTreeNodeExpense* curr = node->children[idx+1];
    while (!curr->is_leaf)
        curr = curr->children[0];
    return curr->keys[0];
}

void fillExpenseChild(BTreeNodeExpense* node, int idx){
    if(idx != 0 && node->children[idx-1]->num_keys >= M/2)
        borrowFromLeftExpense(node, idx);
    else if(idx != node->num_keys && node->children[idx+1]->num_keys >= M/2)
        borrowFromRightExpense(node, idx);
    else{
        if(idx != node->num_keys)
            mergeExpenseNodes(node, idx);
        else
            mergeExpenseNodes(node, idx-1);
    }
}

void borrowFromLeftExpense(BTreeNodeExpense* parent, int idx){
    BTreeNodeExpense* child = parent->children[idx];
    BTreeNodeExpense* sibling = parent->children[idx-1];

    // Shift child's keys right
    for(int i = child->num_keys-1; i >= 0; i--){
        child->keys[i+1] = child->keys[i];
    }
    if(!child->is_leaf){
        for (int i = child->num_keys; i >= 0; i--)
            child->children[i+1] = child->children[i];
    }

    // Move key from parent to child
    child->keys[0] = parent->keys[idx-1];
    if (!child->is_leaf)
        child->children[0] = sibling->children[sibling->num_keys];

    // Move key from sibling to parent
    parent->keys[idx-1] = sibling->keys[sibling->num_keys-1];

    child->num_keys++;
    sibling->num_keys--;
}

void borrowFromRightExpense(BTreeNodeExpense* parent, int idx) {
    BTreeNodeExpense* child = parent->children[idx];
    BTreeNodeExpense* sibling = parent->children[idx+1];

    // Move key from parent to child
    child->keys[child->num_keys] = parent->keys[idx];
    if(!child->is_leaf)
        child->children[child->num_keys+1] = sibling->children[0];

    // Move key from sibling to parent
    parent->keys[idx] = sibling->keys[0];

    // Shift sibling's keys left
    for(int i = 1; i < sibling->num_keys; i++){
        sibling->keys[i-1] = sibling->keys[i];
    }
    if(!sibling->is_leaf){
        for (int i = 1; i <= sibling->num_keys; i++)
            sibling->children[i-1] = sibling->children[i];
    }

    child->num_keys++;
    sibling->num_keys--;
}

void mergeExpenseNodes(BTreeNodeExpense* node, int idx) {
    BTreeNodeExpense* child = node->children[idx];
    BTreeNodeExpense* sibling = node->children[idx+1];

    // Move key from parent to child
    child->keys[M/2-1] = node->keys[idx];

    // Copy keys from sibling to child
    for (int i = 0; i < sibling->num_keys; i++) {
        child->keys[i+M/2] = sibling->keys[i];
    }
    if (!child->is_leaf) {
        for (int i = 0; i <= sibling->num_keys; i++)
            child->children[i+M/2] = sibling->children[i];
    }

    // Shift parent's keys and children
    for (int i = idx+1; i < node->num_keys; i++) {
        node->keys[i-1] = node->keys[i];
    }
    for (int i = idx+2; i <= node->num_keys; i++)
        node->children[i-1] = node->children[i];

    child->num_keys += sibling->num_keys + 1;
    node->num_keys--;

    free(sibling);
}

void deleteFromExpenseSubtree(BTreeNodeExpense* node, int user_id, int expense_id) {
    int idx = find_key_index_expense(node, user_id, expense_id);

    if (idx < node->num_keys && 
        node->keys[idx]->user_id == user_id && 
        node->keys[idx]->expense_id == expense_id){
        if(node->is_leaf){
            removeFromLeafExpense(node, idx);
        }
        else{
            if(node->children[idx]->num_keys >= M/2){
                ExpenseNode* pred = getExpensePredecessor(node, idx);
                node->keys[idx] = pred;
                deleteFromExpenseSubtree(node->children[idx], pred->user_id, pred->expense_id);
            }
            else if(node->children[idx+1]->num_keys >= M/2){
                ExpenseNode* succ = getExpenseSuccessor(node, idx);
                node->keys[idx] = succ;
                deleteFromExpenseSubtree(node->children[idx+1], succ->user_id, succ->expense_id);
            }
            else{
                mergeExpenseNodes(node, idx);
                deleteFromExpenseSubtree(node->children[idx], user_id, expense_id);
            }
        }
    }
    else{
        if(node->is_leaf){
            printf("Expense ID %d for user %d not found.\n", expense_id, user_id);
            return;
        }

        bool flag = (idx == node->num_keys);
        if(node->children[idx]->num_keys < M/2)
            fillExpenseChild(node, idx);

        if(flag && idx > node->num_keys)
            deleteFromExpenseSubtree(node->children[idx-1], user_id, expense_id);
        else
            deleteFromExpenseSubtree(node->children[idx], user_id, expense_id);
    }
}

bool deleteExpense(BTreeNodeExpense** root, int user_id, int expense_id){
    if (*root == NULL) {
        printf("Expense with ID %d for user %d not found!\n", expense_id, user_id);
        return false;
    }

    deleteFromExpenseSubtree(*root, user_id, expense_id);

    if((*root)->num_keys == 0){
        BTreeNodeExpense* temp = *root;
        if ((*root)->is_leaf)
            *root = NULL;
        else
            *root = (*root)->children[0];
        free(temp);
    }

    return true;
}

// Update individual or family details
void updateOrDeleteIndividualFamilyDetails(BTreeNodeUser** user_root,BTreeNodeFamily** family_root,BTreeNodeExpense** expense_root) {
    int choice;
    printf("\n1. Update Individual\n2. Update Family\n3. Delete Individual\n4. Delete Family\nEnter choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: { // Update Individual
            int user_id;
            printf("Enter user ID to update: ");
            scanf("%d", &user_id);
            
            UserNode* user = searchUser(*user_root, user_id);
            if (!user) {
                printf("User not found\n");
                break;
            }

            printf("Current details:\n");
            printUser(user);
            
            // Get new details
            char name[NAME_LEN];
            float income;
            printf("Enter new name (or - to keep): ");
            scanf("%99s", name);
            if (strcmp(name, "-") != 0) {
                strncpy(user->user_name, name, NAME_LEN);
            }
            
            printf("Enter new income (or -1 to keep): ");
            scanf("%f", &income);
            if (income >= 0) {
                // Update family income if in family
                if (user->family) {
                    user->family->total_income += (income - user->income);
                }
                user->income = income;
            }
            
            printf("User updated successfully\n");
            break;
        }
        case 2: { // Update Family
            int family_id;
            printf("Enter family ID to update: ");
            scanf("%d", &family_id);
            
            FamilyNode* family = searchFamily(*family_root, family_id);
            if (!family) {
                printf("Family not found\n");
                break;
            }

            printf("Current details:\n");
            printFamily(family);
            
            char name[NAME_LEN];
            printf("Enter new family name (or - to keep): ");
            scanf("%99s", name);
            if (strcmp(name, "-") != 0) {
                strncpy(family->family_name, name, NAME_LEN);
            }
            
            printf("Family updated successfully\n");
            break;
        }
        case 3: { // Delete Individual
            int user_id;
            printf("Enter user ID to delete: ");
            scanf("%d", &user_id);
        
            UserNode* user = searchUser(*user_root, user_id);
            if (!user) {
                printf("User not found\n");
                break;
            }
        
            // Check if user is in a family and is the last member
            if (user->family && user->family->member_count == 1) {
                // Delete the family first
                deleteFamily(family_root, user->family->family_id); 
            }
        
            // Then delete the user
            if (deleteIndividual(user_root, user_id)) {
                printf("User deleted successfully\n");
            } else {
                printf("Failed to delete user\n");
            }
            break;
        }
        case 4: { // Delete Family (and all its members)
            int family_id;
            printf("Enter family ID to delete: ");
            scanf("%d", &family_id);
        
            FamilyNode* family = searchFamily(*family_root, family_id);
            if (!family) {
                printf("Family not found\n");
                break;
            }
        
            // First delete all members
            for (int i = 0; i < family->member_count; i++) {
                if (family->members[i]) {
                    deleteIndividual(user_root, family->members[i]->user_id);
                }
            }
        
            // Then delete the family
            if (deleteFamily(family_root, family_id)) {
                printf("Family and all members deleted successfully\n");
            } else {
                printf("Failed to delete family\n");
            }
            break;
        }
        default:
            printf("Invalid choice\n");
    }
}

// Update or delete an expense
void updateOrDeleteExpense() {
    int choice;
    printf("\n1. Update Expense\n2. Delete Expense\nEnter choice: ");
    scanf("%d", &choice);

    int user_id, expense_id;
    printf("Enter user ID: ");
    scanf("%d", &user_id);
    printf("Enter expense ID: ");
    scanf("%d", &expense_id);

    UserNode* user = searchUser(user_root, user_id);
    if (!user) {
        printf("User not found\n");
        return;
    }

    ExpenseNode* expense = searchExpenseForUser(user, expense_id);
    if (!expense) {
        printf("Expense not found\n");
        return;
    }

    if (choice == 1) { // Update Expense
        printf("Current expense:\n");
        printExpense(expense);
        
        float amount;
        printf("Enter new amount (or -1 to keep): ");
        scanf("%f", &amount);
        
        int category;
        printf("Enter new category (0-4 or -1 to keep): ");
        scanf("%d", &category);
        
        Date date;
        printf("Enter new date as day month year (or 0 0 0 to keep): ");
        scanf("%d %d %d", &date.day, &date.month, &date.year);
        
        // Calculate differences for updates
        float amount_diff = (amount >= 0) ? (amount - expense->amount) : 0;
        float category_diff[MAX_CATEGORIES] = {0};
        
        if (category >= 0 && category < MAX_CATEGORIES) {
            category_diff[expense->category] -= expense->amount;
            category_diff[category] += (amount >= 0) ? amount : expense->amount;
            expense->category = category;
        }
        
        // Apply updates
        if (amount >= 0) {
            expense->amount = amount;
        }
        if (date.day > 0 && date.month > 0 && date.year > 0) {
            expense->date = date;
        }
        
        // Update user totals
        user->total_expense += amount_diff;
        for (int i = 0; i < MAX_CATEGORIES; i++) {
            user->category_expenses[i] += category_diff[i];
        }
        
        // Update family totals if in family
        if (user->family) {
            user->family->total_expense += amount_diff;
            for (int i = 0; i < MAX_CATEGORIES; i++) {
                user->family->category_expenses[i] += category_diff[i];
            }
        }
        
        printf("Expense updated successfully\n");
    } else if (choice == 2) { // Delete Expense
        if (deleteExpense(&expense_root, user_id, expense_id)) {
            printf("Expense deleted successfully\n");
        } else {
            printf("Failed to delete expense\n");
        }
    } else {
        printf("Invalid choice\n");
    }
}

// Print a single user
void printUser(UserNode* user) {
    if (!user) return;
    
    printf("User ID: %d, Name: %s, Income: %.2f\n", 
           user->user_id, user->user_name, user->income);
    printf("Total Expenses: %.2f\n", user->total_expense);
    printf("Family: %s\n", user->family ? user->family->family_name : "None");
    
    // Print category expenses
    printf("Category Expenses:\n");
    for (int i = 0; i < MAX_CATEGORIES; i++) {
        if (user->category_expenses[i] > 0) {
            printf("  %s: %.2f\n", category_names[i], user->category_expenses[i]);
        }
    }
    printf("\n");
}

// Print a single family
void printFamily(FamilyNode* family) {
    if (!family) return;
    
    printf("Family ID: %d, Name: %s\n", family->family_id, family->family_name);
    printf("Total Income: %.2f, Total Expense: %.2f\n", 
           family->total_income, family->total_expense);
    printf("Members (%d):\n", family->member_count);
    
    for (int i = 0; i < family->member_count; i++) {
        if (family->members[i]) {  // Add null check
            printf("  Member %d: %s (ID: %d)\n", 
                   i+1, family->members[i]->user_name, family->members[i]->user_id);
        }
    }
    
    printf("Family Category Expenses:\n");
    for (int i = 0; i < MAX_CATEGORIES; i++) {
        if (family->category_expenses[i] > 0) {
            printf("  %s: %.2f\n", category_names[i], family->category_expenses[i]);
        }
    }
    printf("\n");
}


// Print a single expense
void printExpense(ExpenseNode* expense) {
    if (!expense) {
        printf("(NULL expense)\n");
        return;
    }
    
    // Validate category
    int category = expense->category;
    if (category < 0 || category >= MAX_CATEGORIES) {
        category = 0;  // Default to first category if invalid
    }
    
    printf("Expense ID: %-5d | User ID: %-5d | Amount: %-8.2f | ", 
           expense->expense_id, expense->user_id, expense->amount);
    printf("Category: %-10s | Date: %02d/%02d/%04d\n",
           category_names[category],
           expense->date.day,
           expense->date.month,
           expense->date.year);
}


// Recursive function to traverse and print all users in the B-tree
void traverseAndPrintUsers(BTreeNodeUser* root) {
    if (root != NULL) {
        int i;
        for (i = 0; i < root->num_keys; i++) {
            traverseAndPrintUsers(root->children[i]);
            printUser(root->keys[i]);
        }
        traverseAndPrintUsers(root->children[i]);
    }
}

// Recursive function to traverse and print all families in the B-tree
void traverseAndPrintFamilies(BTreeNodeFamily* root) {
    if (root != NULL) {
        for (int i = 0; i < root->num_keys; i++) {
            traverseAndPrintFamilies(root->children[i]);
            printFamily(root->keys[i]);
        }
        traverseAndPrintFamilies(root->children[root->num_keys]);
    }
}

// Recursive function to traverse and print all expenses in the B-tree
void traverseAndPrintExpenses(BTreeNodeExpense* root) {
    if (root != NULL) {
        for (int i = 0; i < root->num_keys; i++) {
            traverseAndPrintExpenses(root->children[i]);
            if (root->keys[i] != NULL) {  // Explicit NULL check
                printExpense(root->keys[i]);
            }
        }
        // Don't forget the last child
        if (!root->is_leaf) {
            traverseAndPrintExpenses(root->children[root->num_keys]);
        }
    }
}


// Helper functions to print entire databases
void printAllUsers() {
    printf("\n=== ALL USERS ===\n");
    traverseAndPrintUsers(user_root);
}

void printAllFamilies() {
    printf("\n=== ALL FAMILIES ===\n");
    traverseAndPrintFamilies(family_root);
}

void printAllExpenses() {
    printf("\n=== ALL EXPENSES ===\n");
    traverseAndPrintExpenses(expense_root);
}

void loadDataFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening data file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Skip empty lines
        if (strlen(line) <= 1) continue;
        
        char* token = strtok(line, " \n");
        if (!token) continue;

        if (strcmp(token, "USER") == 0) {
            // USER format: ID Name Income
            int user_id = atoi(strtok(NULL, " \n"));
            char* name = strtok(NULL, " \n");
            float income = atof(strtok(NULL, " \n"));
            addUser(user_id, name, income);
        }
        else if (strcmp(token, "FAMILY") == 0) {
            // FAMILY format: ID Name TotalIncome TotalExpense MemberCount
            int family_id = atoi(strtok(NULL, " \n"));
            char* family_name = strtok(NULL, " \n");
            // Skip totals as we'll calculate them
            strtok(NULL, " \n"); strtok(NULL, " \n"); strtok(NULL, " \n");
            createFamily(family_id, family_name);
        }
        else if (strcmp(token, "MEMBER") == 0) {
            // MEMBER format: FamilyID UserID
            int family_id = atoi(strtok(NULL, " \n"));
            int user_id = atoi(strtok(NULL, " \n"));
            joinFamily(user_id, family_id);
        }
        else if (strcmp(token, "EXPENSE") == 0) {
            // EXPENSE format: ID UserID Amount Category Day Month Year
            int expense_id = atoi(strtok(NULL, " \n"));
            int user_id = atoi(strtok(NULL, " \n"));
            float amount = atof(strtok(NULL, " \n"));
            int category = atoi(strtok(NULL, " \n"));
            Date date;
            date.day = atoi(strtok(NULL, " \n"));
            date.month = atoi(strtok(NULL, " \n"));
            date.year = atoi(strtok(NULL, " \n"));
            addExpense(user_id, expense_id, amount, category, date);
        }
    }
    fclose(file);
}

void freeUserTree(BTreeNodeUser* root) {
    if (root) {
        for (int i = 0; i < root->num_keys; i++) {
            // Free user's expense list first
            ExpenseNode* expense = root->keys[i]->expenses_head;
            while (expense) {
                ExpenseNode* next = expense->next;
                free(expense);
                expense = next;
            }
            // Free the user
            free(root->keys[i]);
        }
        // Recursively free children
        if (!root->is_leaf) {
            for (int i = 0; i <= root->num_keys; i++) {
                freeUserTree(root->children[i]);
            }
        }
        // Free the node itself
        free(root);
    }
}

//Free all Family nodes
void freeFamilyTree(BTreeNodeFamily* root) {
    if (root) {
        for (int i = 0; i < root->num_keys; i++) {
            free(root->keys[i]); // Just free the family, members are freed in user tree
        }
        if (!root->is_leaf) {
            for (int i = 0; i <= root->num_keys; i++) {
                freeFamilyTree(root->children[i]);
            }
        }
        free(root);
    }
}

// 3. Free all Expense nodes (if not already freed with users)
void freeExpenseTree(BTreeNodeExpense* root) {
    if (root) {
        for (int i = 0; i < root->num_keys; i++) {
            free(root->keys[i]);
        }
        if (!root->is_leaf) {
            for (int i = 0; i <= root->num_keys; i++) {
                freeExpenseTree(root->children[i]);
            }
        }
        free(root);
    }
}

// Main menu
int main() {

    loadDataFromFile("data.txt");

    int choice;
    do {
        printf("\n--- Expense Tracking System ---\n");
        printf("1. Add User\n");
        printf("2. Add Expense\n");
        printf("3. Create Family\n");
        printf("4. Join Family\n");
        printf("5. Get Total Family Expense\n");
        printf("6. Get Categorical Expense\n");
        printf("7. Get Highest Expense Day\n");
        printf("8. Get Individual Expense\n");
        printf("9. Get Expenses in Period\n");
        printf("10. Get Expenses in Range\n");
        printf("11. Print Users, Families and Expenses\n");
        printf("12 Update/Delete Family Details\n");
        printf("13 Update/Delete Expense\n");
        printf("14 Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                int user_id;
                char name[NAME_LEN];
                float income;
                printf("Enter user ID: ");
                scanf("%d", &user_id);
                printf("Enter user name: ");
                scanf("%99s", name);
                printf("Enter income: ");
                scanf("%f", &income);

                if (!addUser(user_id, name, income)) {
                    printf("Failed to add user (may already exist)\n");
                }
                break;
            }
            case 2: {
                int user_id, expense_id, category;
                float amount;
                Date date;
                printf("Enter user ID: ");
                scanf("%d", &user_id);
                printf("Enter expense ID: ");
                scanf("%d", &expense_id);
                printf("Enter category (0-Rent, 1-Utility, 2-Grocery, 3-Stationary, 4-Leisure): ");
                scanf("%d", &category);
                printf("Enter amount: ");
                scanf("%f", &amount);
                printf("Enter date (day month year): ");
                scanf("%d %d %d", &date.day, &date.month, &date.year);

                if (!addExpense(user_id, expense_id, amount, category, date)) {
                    printf("Failed to add expense\n");
                }
                break;
            }
            case 3: {
                int family_id;
                char family_name[NAME_LEN];
                printf("Enter family ID: ");
                scanf("%d", &family_id);
                printf("Enter family name: ");
                scanf("%99s", family_name);

                if (!createFamily(family_id, family_name)) {
                    printf("Failed to create family (may already exist)\n");
                }
                break;
            }
            case 4: {
                int user_id, family_id;
                printf("Enter user ID: ");
                scanf("%d", &user_id);
                printf("Enter family ID: ");
                scanf("%d", &family_id);

                if (!joinFamily(user_id, family_id)) {
                    printf("Failed to join family\n");
                }
                break;
            }
            case 5: {
                int family_id;
                printf("Enter family ID: ");
                scanf("%d", &family_id);
                getTotalExpense(family_id);
                break;
            }
            case 6: {
                int family_id, category;
                printf("Enter family ID: ");
                scanf("%d", &family_id);
                printf("Enter category (0-Rent, 1-Utility, 2-Grocery, 3-Stationary, 4-Leisure): ");
                scanf("%d", &category);
                getCategoricalExpense(family_id, category);
                break;
            }
            case 7: {
                int family_id;
                printf("Enter family ID: ");
                scanf("%d", &family_id);
                getHighestExpenseDay(family_id);
                break;
            }
            case 8: {
                int user_id;
                printf("Enter user ID: ");
                scanf("%d", &user_id);
                getIndividualExpense(user_id);
                break;
            }
            case 9: {
                Date start, end;
                printf("Enter start date (day month year): ");
                scanf("%d %d %d", &start.day, &start.month, &start.year);
                printf("Enter end date (day month year): ");
                scanf("%d %d %d", &end.day, &end.month, &end.year);
                getExpensesInPeriod(expense_root,start, end);
                break;
            }
            case 10: {
                int user_id, start_id, end_id;
                printf("Enter user ID: ");
                scanf("%d", &user_id);
                printf("Enter start expense ID: ");
                scanf("%d", &start_id);
                printf("Enter end expense ID: ");
                scanf("%d", &end_id);
                getExpensesInRange(user_id, start_id, end_id);
                break;
            }
            case 11: {
                printf("Printing All Users:\n");
                printAllUsers();
                printf("\n");

                printf("Printing All Families:\n");
                printAllFamilies();
                printf("\n");
                break;
            }
            case 12:{
                updateOrDeleteIndividualFamilyDetails(&user_root,&family_root,&expense_root);
                break;
            }
            case 13:{
                updateOrDeleteExpense();
                break;
            }
            case 14:{
                printf("Exiting...");
                break;
            }
            default: {
                printf("Invalid choice\n");
                break;
            }
        }
    } while (choice != 14);


freeUserTree(user_root);
freeFamilyTree(family_root);
freeExpenseTree(expense_root);

// Reset roots to NULL
user_root = NULL;
family_root = NULL;
expense_root = NULL;

    return 0;
}