#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUEUES 12

typedef struct customer customer;
typedef struct node node;
typedef struct q q;
typedef struct booth booth;

// Struct to hold customer information
struct customer {
    char* name;
    int numberOfTickets;
    int lineNum;
    int arrivalTime;
};

// Struct for a node in the queue
struct node {
    customer* Customer;
    node* nxt;
};

// Struct for a queue
struct q {
    node* head;
    node* tail;
    int sizeOfQ;
};

// Struct for a booth containing multiple queues
struct booth {
    int maxLines;
    q** Lines;
};

// Function to add a customer to the end of a queue
int enqueue(q* qPtr, customer* Customer) {
    node* temp = (node*)malloc(sizeof(node));
    if (temp != NULL) {
        temp->Customer = Customer;
        temp->nxt = NULL;

        if (qPtr->tail != NULL)
            qPtr->tail->nxt = temp;

        qPtr->tail = temp;
        if (qPtr->head == NULL)
            qPtr->head = temp;

        qPtr->sizeOfQ++;
        return 1;
    }
    else
        return 0;
}

// Function to remove a customer from the front of a queue
void dequeue(q* qptr) {
    if (qptr->head == NULL)
        return;

    node* tmp = qptr->head;
    qptr->head = qptr->head->nxt;

    if (qptr->head == NULL)
        qptr->tail = NULL;

    free(tmp);
    qptr->sizeOfQ--;
}

// Function to get the first customer in a queue
customer* peek(q* queue) {
    return queue->head->Customer;
}

// Function to check if a queue is empty
int empty(q* qptr) {
    return qptr->head == NULL ? 1 : 0;
}

// Function to calculate the line number based on the first letter of the customer's name
int calculateLineNum(char firstLetter) {
    return (firstLetter - 'A') % 13;
}

// Function to deallocate memory used by a queue
void deallocate(node** root) {
    node* curr = *root;
    while (curr != NULL) {
        node* temp = curr;
        curr = curr->nxt;
        free(temp->Customer->name);
        free(temp->Customer);
        free(temp);
    }
    *root = NULL;
}

// Function to create a new empty queue
q* createQueue() {
    q* queue = (q*)malloc(sizeof(q));
    queue->head = NULL;
    queue->tail = NULL;
    queue->sizeOfQ = 0;
    return queue;
}

// Function to get the index of the first non-empty queue in a booth
int getnonEmptyQueue(booth* Booths, int numberOfBooth) {
    int queueCount = 0;
    for (int i = 0; i < numberOfBooth; i++) {
        int maxlines = Booths[i].maxLines;
        q** lines = Booths[i].Lines;
        for (int j = 0; j < maxlines; j++) {
            if (lines[j]->sizeOfQ > 0)
                return queueCount;
            queueCount++;
        }
    }
    return queueCount;
}

int main() {
    int numPeople = 0;
    int numBooths = 0;
    int queuesPerBooth = 0;
    int remainderQueues = 0;
    booth Booths[12];

    scanf("%d", &numPeople);
    scanf("%d", &numBooths);

    queuesPerBooth = MAX_QUEUES / numBooths;
    remainderQueues = MAX_QUEUES % numBooths;

    // Initialize booths and queues
    for (int i = 0; i < numBooths; i++) {
        int currentQueueAmount = queuesPerBooth;
        if (remainderQueues > 0) {
            currentQueueAmount++;
            remainderQueues--;
        }
        Booths[i].Lines = (q**)malloc(sizeof(q*) * currentQueueAmount);
        Booths[i].maxLines = currentQueueAmount;
        for (int j = 0; j < currentQueueAmount; j++) {
            Booths[i].Lines[j] = createQueue();
        }
    }

    // Process each customer
    for (int i = 0; i < numPeople; i++) {
        char name[50];
        int numTickets;
        int arrivalTime;

        scanf("%s %d %d", name, &numTickets, &arrivalTime);

        customer* Customer = (customer*)malloc(sizeof(customer));
        Customer->name = strdup(name);
        Customer->numberOfTickets = numTickets;
        Customer->arrivalTime = arrivalTime;

        // Calculate the queue position based on the first letter of the name
        int queuePosition = calculateLineNum(name[0]);

        // If the calculated queue position is 0, find the first non-empty queue
        if (queuePosition == 0) {
            queuePosition = getnonEmptyQueue(Booths, numBooths);
        }
        int tempqueuepos = queuePosition;

        // Enqueue the customer in the appropriate queue
        for (int j = 0; j < numBooths; j++) {
            if (tempqueuepos < Booths[j].maxLines) {
                Customer->lineNum = queuePosition;
                enqueue(Booths[j].Lines[tempqueuepos], Customer);
                break;
            }
            tempqueuepos = tempqueuepos - Booths[j].maxLines;
        }
    }

    // Process each booth
    for (int i = 0; i < numBooths; i++) {
        printf("Booth %d\n", i + 1);
        int maxLines = Booths[i].maxLines;
        q** lines = Booths[i].Lines;
        int checkoutTime = 0;
        // Process each queue in the booth
        for (int j = 0; j < maxLines; j++) {
            if (empty(lines[j]))
                continue;

            customer* frontCustomer = peek(lines[j]);

            if (j == 0) {
                checkoutTime = 5 * frontCustomer->numberOfTickets + 30 + frontCustomer->arrivalTime;
            }
            else {
                checkoutTime += 5 * frontCustomer->numberOfTickets + 30;
            }

            // Print the checkout information for the customer
            printf("%s from line %d checks out at time %d\n", frontCustomer->name, frontCustomer->lineNum, checkoutTime);
            dequeue(lines[j]);
        }
    }

    // Deallocate memory used by booths and queues
    for (int i = 0; i < numBooths; i++) {
        int maxLines = Booths[i].maxLines;
        for (int j = 0; j < maxLines; j++) {
            deallocate(&(Booths[i].Lines[j]->head));
            free(Booths[i].Lines[j]);
        }
        free(Booths[i].Lines);
    }

    return 0;
}
