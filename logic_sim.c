
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_EXPR_LENGTH 100 // max length of the expression
#define MAX_VARS 10 // max number of variables



// Logic gate functions
int AND(int a, int b) { return a & b; }
int OR(int a, int b) { return a | b; }
int NOT(int a) { return !a; }
int XOR(int a, int b) { return a ^ b; }
int NAND(int a, int b) { return !(a & b); }
int NOR(int a, int b) { return !(a | b); }
int XNOR(int a, int b) { return !(a ^ b); }

// Operator precedence
int precedence(char* op) {
    if (strcmp(op, "AND") == 0) return 2;
    if (strcmp(op, "OR") == 0) return 1;
    if (strcmp(op, "XOR") == 0) return 2;
    if (strcmp(op, "NAND") == 0) return 2;
    if (strcmp(op, "NOR") == 0) return 1;
    if (strcmp(op, "XNOR") == 0) return 2;
    return 0;
}

// Logic expression evaluator
int apply_operator(int a, int b, char* op) {
    if (strcmp(op, "AND") == 0) return AND(a, b);
    if (strcmp(op, "OR") == 0) return OR(a, b);
    if (strcmp(op, "XOR") == 0) return XOR(a, b);
    if (strcmp(op, "NAND") == 0) return NAND(a, b);
    if (strcmp(op, "NOR") == 0) return NOR(a, b);
    if (strcmp(op, "XNOR") == 0) return XNOR(a, b);
    return 0;
}

// Tokenize the expression based on space
void tokenize(char* expr, char tokens[MAX_EXPR_LENGTH][MAX_EXPR_LENGTH], int* token_count) {
    char* token = strtok(expr, " ");
    *token_count = 0;
    while (token != NULL) {
        strcpy(tokens[*token_count], token);
        (*token_count)++;
        token = strtok(NULL, " ");
    }
}

// Evaluate the expression
int evaluate_expression(char* expr) {
    char tokens[MAX_EXPR_LENGTH][MAX_EXPR_LENGTH];
    int token_count = 0;
    tokenize(expr, tokens, &token_count);

    int values[MAX_EXPR_LENGTH];
    char operators[MAX_EXPR_LENGTH][MAX_EXPR_LENGTH];
    int val_top = -1, op_top = -1;

    for (int i = 0; i < token_count; i++) {
        if (isdigit(tokens[i][0])) {
            values[++val_top] = atoi(tokens[i]);
        }
        else if (strcmp(tokens[i], "NOT") == 0) {
            i++;
            int val = atoi(tokens[i]);
            values[++val_top] = NOT(val);
        }
        else if (strcmp(tokens[i], "(") == 0) {
            operators[++op_top][0] = '(';
        }
        else if (strcmp(tokens[i], ")") == 0) {
            while (op_top >= 0 && operators[op_top][0] != '(') {
                int b = values[val_top--];
                int a = values[val_top--];
                char op[4];
                strcpy(op, operators[op_top--]);
                values[++val_top] = apply_operator(a, b, op);
            }
            op_top--;  // Pop '('
        }
        else {  // Operator (AND, OR, etc.)
            while (op_top >= 0 && precedence(operators[op_top]) >= precedence(tokens[i])) {
                int b = values[val_top--];
                int a = values[val_top--];
                char op[4];
                strcpy(op, operators[op_top--]);
                values[++val_top] = apply_operator(a, b, op);
            }
            strcpy(operators[++op_top], tokens[i]);
        }
    }

    while (op_top >= 0) {
        int b = values[val_top--];
        int a = values[val_top--];
        char op[4];
        strcpy(op, operators[op_top--]);
        values[++val_top] = apply_operator(a, b, op);
    }

    return values[val_top];
}

// Find unique variables in the expression
int find_variables(char* expr, char vars[MAX_VARS]) {
    int count = 0;
    int found[26] = { 0 }; // Track already found variables (A-Z)

    for (int i = 0; expr[i] != '\0'; i++) {
        char c = expr[i];

        // Check if it's a standalone capital letter (A-Z)
        if (c >= 'A' && c <= 'Z') {
            // Ensure it's not part of a word like "AND" or "OR"
            if ((i > 0 && isalpha(expr[i - 1])) || (i < strlen(expr) - 1 && isalpha(expr[i + 1]))) {
                continue; // Skip if inside a word
            }

            // If not already added, store it as a variable
            if (!found[c - 'A']) {
                vars[count++] = c;
                found[c - 'A'] = 1; // Mark as found
            }
        }
    }

    return count;
}

// Replace variables with values from the truth table row
void replace_vars(const char* expr, char vars[MAX_VARS], int values[MAX_VARS], int var_count, char* modified_expr) {
    int i, j = 0;
    while (*expr) {
        int replaced = 0;
        // Check if the current character is a variable
        for (i = 0; i < var_count; i++) {
            if (*expr == vars[i] &&
                (expr == expr || !isalpha(*(expr - 1))) && // Ensure it's not part of a word
                (!isalpha(*(expr + 1)))) { // Ensure it's not part of a word
                modified_expr[j++] = values[i] + '0'; // Replace with 0 or 1
                replaced = 1;
                break;
            }
        }
        // If not replaced, copy character as is
        if (!replaced) {
            modified_expr[j++] = *expr;
        }
        expr++;
    }
    modified_expr[j] = '\0'; // Null-terminate
}

// Generate truth table
void generate_truth_table(char* expr) {
    char vars[MAX_VARS];
    int var_count = find_variables(expr, vars);

    if (var_count == 0) {
        printf("Error: No variables found!\n");
        return;
    }

    printf("\nTruth Table for: %s\n", expr);

    // Print table header
    for (int i = 0; i < var_count; i++) {
        printf("%c ", vars[i]);
    }
    printf("| Result\n");
    printf("-----------------\n");

    int rows = 1 << var_count; // 2^var_count

    for (int i = 0; i < rows; i++) {
        int values[MAX_VARS];

        // Assign 0s and 1s to variables
        for (int j = 0; j < var_count; j++) {
            values[j] = (i >> (var_count - j - 1)) & 1;
            printf("%d ", values[j]);
        }

        // Replace variables with current values
        char modified_expr[MAX_EXPR_LENGTH];
        replace_vars(expr, vars, values, var_count, modified_expr);

        // Evaluate the modified expression
        int result = evaluate_expression(modified_expr);
        printf("| %d\n", result);
    }
}

int main() {
    char expr[MAX_EXPR_LENGTH];

    printf("Enter a logic expression using variables (e.g., 'A AND B OR C'):\n");
    fgets(expr, MAX_EXPR_LENGTH, stdin); // Read expression from user
    expr[strcspn(expr, "\n")] = 0;  // Remove newline

    generate_truth_table(expr);
    return 0;
}


