#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTS 100
#define FILE_NAME "products.txt"

typedef struct {
    int id;
    char name[50];
    char category[20];
    char type[20];
    float price;
} Product;

Product products[MAX_PRODUCTS];
int productCount = 0;

void clearInput(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int inputInt(const char *prompt, int *value)
{
    printf("%s", prompt);
    if (scanf("%d", value) != 1) {
        clearInput();
        printf("Invalid input.\n");
        return 0;
    }
    clearInput();
    return 1;
}

int inputFloat(const char *prompt, float *value)
{
    printf("%s", prompt);
    if (scanf("%f", value) != 1) {
        clearInput();
        printf("Invalid input.\n");
        return 0;
    }
    clearInput();
    return 1;
}

void inputString(const char *prompt, char *text, int size)
{
    printf("%s", prompt);
    fgets(text, size, stdin);
    text[strcspn(text, "\n")] = '\0';
}

int findProduct(int id)
{
    int i;
    for (i = 0; i < productCount; i++) {
        if (products[i].id == id) return i;
    }
    return -1;
}

void saveProducts(void)
{
    FILE *file = fopen(FILE_NAME, "w");
    int i;

    if (file == NULL) {
        printf("Error saving products.\n");
        return;
    }

    for (i = 0; i < productCount; i++) {
        fprintf(file, "%d\n", products[i].id);
        fprintf(file, "%s\n", products[i].name);
        fprintf(file, "%s\n", products[i].category);
        fprintf(file, "%s\n", products[i].type);
        fprintf(file, "%.2f\n", products[i].price);
    }

    fclose(file);
}

void loadProducts(void)
{
    FILE *file = fopen(FILE_NAME, "r");
    int i;

    if (file == NULL)
        return;

    productCount = 0;

    while (productCount < MAX_PRODUCTS) {
        Product p;
        char line[100];

        if (fgets(line, sizeof(line), file) == NULL)
            break;
        if (sscanf(line, "%d", &p.id) != 1)
            break;

        if (fgets(p.name, sizeof(p.name), file) == NULL)
            break;
        p.name[strcspn(p.name, "\n")] = '\0';

        if (fgets(p.category, sizeof(p.category), file) == NULL)
            break;
        p.category[strcspn(p.category, "\n")] = '\0';

        if (fgets(p.type, sizeof(p.type), file) == NULL)
            break;
        p.type[strcspn(p.type, "\n")] = '\0';

        if (fgets(line, sizeof(line), file) == NULL)
            break;
        if (sscanf(line, "%f", &p.price) != 1)
            break;

        if (p.id < 0 || p.name[0] == '\0' || p.price < 0)
            continue;

        for (i = 0; i < productCount; i++) {
            if (products[i].id == p.id)
                break;
        }

        if (i < productCount)
            continue;

        products[productCount] = p;
        productCount++;
    }

    fclose(file);
}

void showProducts(const char *category, const char *type)
{
    int i, found = 0;

    printf("\n%-5s %-20s %-12s %-12s %-10s\n",
           "ID", "NAME", "CATEGORY", "TYPE", "PRICE");
    printf("---------------------------------------------------------------\n");

    for (i = 0; i < productCount; i++) {
        if (strcmp(products[i].category, category) == 0 &&
            (type == NULL || strcmp(products[i].type, type) == 0)) {

            printf("%-5d %-20s %-12s %-12s %-10.2f\n",
                   products[i].id, products[i].name,
                   products[i].category, products[i].type,
                   products[i].price);
            found = 1;
        }
    }

    if (!found) printf("No products found.\n");
}

int chooseType(const char *category, char *type)
{
    int choice;

    if (strcmp(category, "SNACKS") == 0) {
        strcpy(type, "-");
        return 1;
    }

    if (strcmp(category, "DRINKS") == 0) {
        printf("\n1. ICED\n2. HOT\n");
        if (!inputInt("Enter choice: ", &choice)) return 0;

        if (choice == 1) strcpy(type, "ICED");
        else if (choice == 2) strcpy(type, "HOT");
        else return 0;
    } else {
        printf("\n1. BREAKFAST\n2. LUNCH\n3. DINNER\n");
        if (!inputInt("Enter choice: ", &choice)) return 0;

        if (choice == 1) strcpy(type, "BREAKFAST");
        else if (choice == 2) strcpy(type, "LUNCH");
        else if (choice == 3) strcpy(type, "DINNER");
        else return 0;
    }

    return 1;
}

int chooseCategory(char *category, char *type)
{
    int choice;

    printf("\n1. DRINKS\n2. SNACKS\n3. FOODS\n");

    if (!inputInt("Enter choice: ", &choice)) return 0;

    if (choice == 1) strcpy(category, "DRINKS");
    else if (choice == 2) strcpy(category, "SNACKS");
    else if (choice == 3) strcpy(category, "FOODS");
    else return 0;

    return chooseType(category, type);
}

int confirmAction(const char *prompt)
{
    char answer[10];
    inputString(prompt, answer, sizeof(answer));
    return answer[0] == 'Y' || answer[0] == 'y';
}

void addProduct(void)
{
    Product *p;
    int id;

    if (productCount >= MAX_PRODUCTS) {
        printf("Product limit reached.\n");
        return;
    }

    printf("ADD PRODUCT\n");

    while (1) {
        if (!inputInt("Enter Product ID: ", &id))
            continue;

        if (id < 0) {
            printf("Product ID cannot be negative.\n");
            continue;
        }

        if (findProduct(id) != -1) {
            printf("Product ID already exists. Enter another ID.\n");
            continue;
        }

        break;
    }

    p = &products[productCount];
    p->id = id;

    inputString("Enter Product Name: ", p->name, sizeof(p->name));

    if (p->name[0] == '\0') {
        printf("Product name cannot be empty.\n");
        return;
    }

    if (!chooseCategory(p->category, p->type)) {
        printf("Invalid category/type.\n");
        return;
    }

    if (!inputFloat("Enter Price: ", &p->price)) return;

    if (p->price < 0) {
        printf("Price cannot be negative.\n");
        return;
    }

    productCount++;
    saveProducts();
    printf("Product added successfully.\n\n");
}

void updateProduct(void)
{
    int id, index, choice;
    float price;
    char name[50];

    printf("UPDATE PRODUCT\n");

    if (!inputInt("Enter Product ID: ", &id)) return;

    index = findProduct(id);

    if (index == -1) {
        printf("Product not found.\n");
        return;
    }

    printf("\nProduct: %s\n", products[index].name);
    printf("Price: %.2f\n", products[index].price);

    if (!confirmAction(
        "Are you sure you want to update this product? (Y/N): ")) {
        printf("Update cancelled.\n");
        return;
    }

    do {
        printf("\n1. Update Name\n2. Update Price\n0. BACK\n");

        if (!inputInt("Enter choice: ", &choice)) continue;

        if (choice == 1) {
            inputString("Enter New Name: ", name, sizeof(name));

            if (name[0] == '\0') {
                printf("Name cannot be empty.\n");
            } else {
                strcpy(products[index].name, name);
                saveProducts();
                printf("Name updated successfully.\n");
            }
        } else if (choice == 2) {
            if (!inputFloat("Enter New Price: ", &price)) continue;

            if (price < 0) {
                printf("Price cannot be negative.\n");
            } else {
                products[index].price = price;
                saveProducts();
                printf("Price updated successfully.\n");
            }
        } else if (choice != 0) {
            printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

void viewProducts(void)
{
    int choice, typeChoice;
    const char *category, *type;

    do {
        printf("VIEW PRODUCTS\n");
        printf("1. DRINKS\n2. SNACKS\n3. FOODS\n0. BACK\n");

        if (!inputInt("Enter choice: ", &choice)) continue;
        if (choice == 0) break;

        if (choice == 2) {
            showProducts("SNACKS", "-");
            continue;
        }

        if (choice == 1) category = "DRINKS";
        else if (choice == 3) category = "FOODS";
        else {
            printf("Invalid choice.\n");
            continue;
        }

        do {
            if (choice == 1)
                printf("\n1. ICED\n2. HOT\n0. BACK\n");
            else
                printf("\n1. BREAKFAST\n2. LUNCH\n3. DINNER\n0. BACK\n");

            if (!inputInt("Enter choice: ", &typeChoice)) continue;
            if (typeChoice == 0) break;

            type = NULL;

            if (choice == 1) {
                if (typeChoice == 1) type = "ICED";
                else if (typeChoice == 2) type = "HOT";
            } else {
                if (typeChoice == 1) type = "BREAKFAST";
                else if (typeChoice == 2) type = "LUNCH";
                else if (typeChoice == 3) type = "DINNER";
            }

            if (type != NULL) showProducts(category, type);
            else printf("Invalid choice.\n");

        } while (typeChoice != 0);

    } while (choice != 0);
}

void removeProduct(void)
{
    int id, index, i;

    printf("REMOVE PRODUCT\n");

    if (!inputInt("Enter Product ID: ", &id)) return;

    index = findProduct(id);

    if (index == -1) {
        printf("Product not found.\n");
        return;
    }

    printf("\nProduct: %s\n", products[index].name);
    printf("Price: %.2f\n", products[index].price);

    if (!confirmAction(
        "Are you sure you want to remove this product? (Y/N): ")) {
        printf("Remove cancelled.\n");
        return;
    }

    for (i = index; i < productCount - 1; i++)
        products[i] = products[i + 1];

    productCount--;
    saveProducts();
    printf("Product removed successfully.\n");
}

void sortProducts(void)
{
    int i, j;
    Product temp;

    if (productCount == 0) {
        printf("\nNo products available.\n");
        return;
    }

    for (i = 0; i < productCount - 1; i++) {
        for (j = 0; j < productCount - i - 1; j++) {
            if (products[j].price > products[j + 1].price) {
                temp = products[j];
                products[j] = products[j + 1];
                products[j + 1] = temp;
            }
        }
    }

    saveProducts();

    printf("\nLISTINGS\n");
    printf("Lowest to highest price.\n");
    printf("\n%-5s %-20s %-12s %-12s %-10s\n",
           "ID", "NAME", "CATEGORY", "TYPE", "PRICE");
    printf("---------------------------------------------------------------\n");

    for (i = 0; i < productCount; i++) {
        printf("%-5d %-20s %-12s %-12s %-10.2f\n",
               products[i].id, products[i].name,
               products[i].category, products[i].type,
               products[i].price);
    }
    printf("\n");
}

int validCard(const char *card)
{
    int i;

    if (card[0] == '\0') return 0;

    for (i = 0; card[i] != '\0'; i++)
        if (!isdigit((unsigned char)card[i])) return 0;

    return 1;
}

void transaction(void)
{
    int id, index, quantity, paymentChoice;
    int paymentSuccessful = 0;
    float total, amount;
    char card[50];

    printf("\nTRANSACTION\n");

    if (!inputInt("Enter Product ID: ", &id)) return;

    index = findProduct(id);

    if (index == -1) {
        printf("Product not found.\n");
        return;
    }

    printf("Product: %s\n", products[index].name);
    printf("Price: %.2f\n", products[index].price);

    if (!inputInt("Enter Quantity: ", &quantity) || quantity <= 0) {
        printf("Invalid quantity.\n");
        return;
    }

    total = products[index].price * quantity;
    printf("Total: %.2f\n", total);

    printf("\n1. CASH\n2. CASHLESS\n0. CANCEL\n");

    if (!inputInt("Enter payment choice: ", &paymentChoice)) return;

    if (paymentChoice == 1) {
        if (!inputFloat("Enter Cash Amount: ", &amount)) return;

        if (amount < 0) {
            printf("Payment amount cannot be negative.\n");
            return;
        }

        if (amount < total) {
            printf("Insufficient payment.\n");
            return;
        }

        paymentSuccessful = 1;
        printf("Payment successful.\n");

        if (amount > total)
            printf("Change: %.2f\n", amount - total);

    } else if (paymentChoice == 2) {
        inputString("Enter Card Number: ", card, sizeof(card));

        if (!validCard(card)) {
            printf("Invalid card number. Please enter digits only.\n");
            return;
        }

        if (!inputFloat("Enter Amount: ", &amount)) return;

        if (amount < 0) {
            printf("Payment amount cannot be negative.\n");
            return;
        }

        if (amount < total) {
            printf("Insufficient payment.\n");
            return;
        }

        paymentSuccessful = 1;
        printf("Payment successful.\n");

    } else if (paymentChoice == 0) {
        printf("Transaction cancelled.\n");
        return;
    } else {
        printf("Invalid payment choice.\n");
        return;
    }

    if (paymentSuccessful) {
        printf("TRANSACTION RECEIPT");
        printf("Product : %s\n", products[index].name);
        printf("Price   : %.2f\n", products[index].price);
        printf("Quantity: %d\n", quantity);
        printf("Total   : %.2f\n", total);
        printf("Status  : PAID\n");
    }
}

int main(void)
{
    int choice;

    loadProducts();

    do {
        printf("SMARBITE\n");
        printf("1. Add Product\n2. Update Product\n3. View Product\n4. Remove Product\n5. Sorted List (L-H)\n6. Transaction\n0. Exit\n");


        if (!inputInt("Enter choice: ", &choice)) continue;
        printf("\n");

        switch (choice) {
            case 1: addProduct(); break;
            case 2: updateProduct(); break;
            case 3: viewProducts(); break;
            case 4: removeProduct(); break;
            case 5: sortProducts(); break;
            case 6: transaction(); break;
            case 0: printf("Thank you for using SMARBITE!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}
