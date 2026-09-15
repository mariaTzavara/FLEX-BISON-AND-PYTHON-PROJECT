/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Η yylex() δημιουργείται από το Flex.
 * Ο parser την καλεί κάθε φορά που χρειάζεται
 * να διαβάσει το επόμενο token.
*/
extern int yylex();

/*
 * Η μεταβλητή line δηλώνεται στο lexer.l και κρατά
 * τον αριθμό της τρέχουσας γραμμής του αρχείου εισόδου.
*/
extern int line;

/*
 * Το yyin είναι το αρχείο από το οποίο διαβάζει ο lexer.
 * Δηλώνεται από το Flex και εδώ το συνδέουμε με το αρχείο
 * που δίνει ο χρήστης ως όρισμα.
*/
extern FILE *yyin;

int semantic_errors = 0;
int create_start_errors = 0;

/* Συνάρτηση που καλείται όταν ο Bison εντοπίσει συντακτικό λάθος. */
void yyerror(const char *s);

#define MAX_TABLES 100
#define MAX_COLUMNS 100

/* Τύποι δεδομένων στηλών και κυριολεκτικών. */
enum DataType {
    TYPE_INT = 1,
    TYPE_FLOAT = 2,
    TYPE_STRING = 4
};

typedef struct {
    char *name;

    /* Ονόματα στηλών του πίνακα. */
    char *columns[MAX_COLUMNS];

    /* Τύπος κάθε αντίστοιχης στήλης. */
    int column_types[MAX_COLUMNS];

    int column_count;
} TableInfo;

/* Αποθηκεύει όλους τους πίνακες που έχουν δημιουργηθεί. */
TableInfo created_tables[MAX_TABLES];

/* Πλήθος πινάκων που έχουν δημιουργηθεί. */
int table_count = 0;

/*
 * Προσωρινή λίστα στηλών της CREATE TABLE
 * που αναλύεται αυτή τη στιγμή.
 */
char *current_columns[MAX_COLUMNS];

/* Τύπος κάθε προσωρινής στήλης της CREATE TABLE. */
int current_column_types[MAX_COLUMNS];

/*
 * Ο πίνακας που χρησιμοποιείται στο FROM της SELECT
 * που αναλύεται αυτή τη στιγμή.
 */
char *current_select_table = NULL;

/*
 * Πίνακες που είναι διαθέσιμοι στην τρέχουσα SELECT:
 * ο πίνακας του FROM και οι πίνακες των JOIN.
 */
char *available_tables[MAX_TABLES];

/*
 * Το alias κάθε αντίστοιχου πίνακα.
 *
 * Αν ένας πίνακας δεν έχει alias, η αντίστοιχη θέση
 * περιέχει NULL.
 *
 * Παράδειγμα:
 *
 * available_tables[0] = "students"
 * available_aliases[0] = "s"
 */
char *available_aliases[MAX_TABLES];

int available_table_count = 0;

/* Ο πίνακας του JOIN που αναλύεται αυτή τη στιγμή. */
char *current_join_table = NULL;

int current_column_count = 0;

typedef struct {
    char *qualifier;
    char *column_name;
    int literal_types;
} ColumnReference;

ColumnReference used_columns[MAX_COLUMNS];
int used_column_count = 0;


/*
 * Αναζητά έναν πίνακα με βάση το όνομά του.
 *
 * Επιστρέφει τη θέση του στον πίνακα created_tables,
 * ή -1 αν δεν υπάρχει.
 */
int find_table(const char *name)
{
    int i;

    for (i = 0; i < table_count; i++) {
        if (strcmp(created_tables[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}

/* Επιστρέφει 1 αν ο πίνακας υπάρχει, διαφορετικά 0. */
int table_exists(const char *name)
{
    return find_table(name) != -1;
}

/*
 * Ελέγχει αν ένα όνομα στήλης υπάρχει ήδη στην προσωρινή
 * λίστα της CREATE TABLE που αναλύεται.
 */
int current_column_exists(const char *name)
{
    int i;

    for (i = 0; i < current_column_count; i++) {
        if (strcmp(current_columns[i], name) == 0) {
            return 1;
        }
    }

    return 0;
}


/*
 * Προσθέτει προσωρινά μία στήλη της CREATE TABLE
 * μαζί με τον τύπο δεδομένων της.
 */
void add_current_column(const char *name, int data_type)
{
    if (current_column_count >= MAX_COLUMNS) {
        fprintf(stderr,
                "Σφάλμα: ξεπεράστηκε το μέγιστο πλήθος στηλών.\n");
        semantic_errors++;
        return;
    }

    current_columns[current_column_count] = strdup(name);
    current_column_types[current_column_count] = data_type;
    current_column_count++;
}

/*
 * Αποθηκεύει έναν καινούργιο πίνακα μαζί με τις στήλες
 * που συγκεντρώθηκαν κατά την ανάλυση της CREATE TABLE.
 */
void add_table(const char *name)
{
    int i;

    if (table_count >= MAX_TABLES) {
        fprintf(stderr,
                "Σφάλμα: δεν υπάρχει χώρος για άλλους πίνακες.\n");
        semantic_errors++;
        return;
    }

    created_tables[table_count].name = strdup(name);
    created_tables[table_count].column_count = current_column_count;

    for (i = 0; i < current_column_count; i++) {
    created_tables[table_count].columns[i] =
        strdup(current_columns[i]);

    created_tables[table_count].column_types[i] =
        current_column_types[i];
    }

    table_count++;
}

/*
 * Επιστρέφει 1 αν η συγκεκριμένη στήλη έχει δηλωθεί
 * στον συγκεκριμένο πίνακα.
 */
int column_exists_in_table(const char *table_name,
                           const char *column_name)
{
    int table_index;
    int i;

    table_index = find_table(table_name);

    if (table_index == -1) {
        return 0;
    }

    for (i = 0;
         i < created_tables[table_index].column_count;
         i++) {

        if (strcmp(
                created_tables[table_index].columns[i],
                column_name
            ) == 0) {
            return 1;
        }
    }

    return 0;
}

/* Καθαρίζει την προσωρινή λίστα στηλών της CREATE TABLE. */
void clear_current_columns(void)
{
    int i;

    for (i = 0; i < current_column_count; i++) {
        free(current_columns[i]);
    }

    current_column_count = 0;
}


/*
 * Αποθηκεύει προσωρινά μία αναφορά στήλης.
 *
 * qualifier:
 * - alias ή όνομα πίνακα για s.name/students.name
 * - NULL για μη χαρακτηρισμένη στήλη, π.χ. name
 */
void add_used_column(const char *qualifier,
                     const char *name,
                     int literal_types)
{
    if (used_column_count >= MAX_COLUMNS) {
        fprintf(stderr,
                "Σφάλμα: ξεπεράστηκε το μέγιστο πλήθος "
                "στηλών στη SELECT.\n");

        semantic_errors++;
        return;
    }

    used_columns[used_column_count].qualifier =
        qualifier == NULL ? NULL : strdup(qualifier);

    used_columns[used_column_count].column_name =
        strdup(name);

    used_columns[used_column_count].literal_types =
        literal_types;

    used_column_count++;
}

/* Καθαρίζει τις προσωρινές στήλες της SELECT. */
void clear_used_columns(void)
{
    int i;

    for (i = 0; i < used_column_count; i++) {
        free(used_columns[i].qualifier);
        free(used_columns[i].column_name);
    }

    used_column_count = 0;
}


const char *resolve_qualifier(const char *qualifier,int report_error);

void check_where_types(const char *table_name,const char *column_name,int literal_types);

/*
 * Ελέγχει μία στήλη που εμφανίστηκε σε SELECT, WHERE,
 * GROUP BY ή ORDER BY.
 */
void check_column_reference(const ColumnReference *ref)
{
    int i;
    int matches = 0;
    int matched_index = -1;

    const char *real_table = NULL;

    /*
     * Περίπτωση χαρακτηρισμένης στήλης:
     *
     * s.name
     * students.name
     */
    if (ref->qualifier != NULL) {
        real_table = resolve_qualifier(ref->qualifier, 1);

        if (real_table == NULL) {
            return;
        }

        if (!column_exists_in_table(real_table,
                                    ref->column_name)) {
            semantic_errors++;

            fprintf(stderr,
                    "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                    "η στήλη '%s' δεν υπάρχει στον πίνακα '%s'.\n",
                    line, ref->column_name, real_table);

            return;
        }
    }
    /*
     * Περίπτωση μη χαρακτηρισμένης στήλης:
     *
     * name
     */
    else {
        for (i = 0; i < available_table_count; i++) {
            if (column_exists_in_table(
                    available_tables[i],
                    ref->column_name)) {

                matches++;
                matched_index = i;
            }
        }

        if (matches == 0) {
            semantic_errors++;

            fprintf(stderr,
                    "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                    "η στήλη '%s' δεν υπάρχει σε κανέναν "
                    "πίνακα της SELECT.\n",
                    line, ref->column_name);

            return;
        }

        if (matches > 1) {
            semantic_errors++;

            fprintf(stderr,
                    "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                    "η στήλη '%s' είναι αμφίσημη. "
                    "Χρειάζεται πρόθεμα πίνακα ή alias.\n",
                    line, ref->column_name);

            return;
        }

        /*
         * Αν ο μοναδικός πίνακας στον οποίο ανήκει η στήλη
         * έχει alias, είναι υποχρεωτικό να γραφτεί alias.column.
         */
        if (available_aliases[matched_index] != NULL) {
            semantic_errors++;

            fprintf(stderr,
                    "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                    "η στήλη '%s' ανήκει στον πίνακα '%s' "
                    "με alias '%s'. Πρέπει να γραφτεί '%s.%s'.\n",
                    line,
                    ref->column_name,
                    available_tables[matched_index],
                    available_aliases[matched_index],
                    available_aliases[matched_index],
                    ref->column_name);

            return;
        }

        real_table = available_tables[matched_index];
    }

    /*
     * Αν η στήλη εμφανίζεται μέσα σε WHERE, ελέγχουμε
     * και τη συμβατότητα του τύπου της.
     */
    if (ref->literal_types != 0) {
        check_where_types(real_table,
                          ref->column_name,
                          ref->literal_types);
    }
}

/*
 * Ελέγχει ότι κάθε μη χαρακτηρισμένη στήλη της SELECT,
 * του WHERE, του GROUP BY ή του ORDER BY υπάρχει σε
 * τουλάχιστον έναν από τους διαθέσιμους πίνακες.
 */
void check_used_columns(void)
{
    int i;

    for (i = 0; i < used_column_count; i++) {
        check_column_reference(&used_columns[i]);
    }
}

/*
 * Επιστρέφει τον τύπο μιας στήλης του συγκεκριμένου πίνακα.
 * Επιστρέφει 0 αν ο πίνακας ή η στήλη δεν υπάρχει.
 */
int get_column_type(const char *table_name,
                    const char *column_name)
{
    int table_index;
    int i;

    table_index = find_table(table_name);

    if (table_index == -1) {
        return 0;
    }

    for (i = 0;
         i < created_tables[table_index].column_count;
         i++) {

        if (strcmp(created_tables[table_index].columns[i],
                   column_name) == 0) {

            return created_tables[table_index].column_types[i];
        }
    }

    return 0;
}

/* Μετατρέπει έναν τύπο σε κείμενο για τα μηνύματα λάθους. */
const char *type_to_string(int data_type)
{
    switch (data_type) {
        case TYPE_INT:
            return "INT";

        case TYPE_FLOAT:
            return "FLOAT";

        case TYPE_STRING:
            return "STRING";

        default:
            return "άγνωστος τύπος";
    }
}

/*
 * Ελέγχει αν ένας τύπος στήλης είναι συμβατός με
 * τους τύπους των κυριολεκτικών.
 *
 * Για value_list, το literal_types μπορεί να περιέχει
 * περισσότερους από έναν τύπους.
 */
int types_are_compatible(int column_type, int literal_types)
{
    if (column_type == TYPE_INT) {
        /* Η INT συγκρίνεται μόνο με ακέραια κυριολεκτικά. */
        return literal_types == TYPE_INT;
    }

    if (column_type == TYPE_FLOAT) {
        /*
         * Η FLOAT συγκρίνεται με ακέραια ή πραγματικά
         * κυριολεκτικά, ή με συνδυασμό αυτών μέσα σε IN.
         */
        return (literal_types & TYPE_STRING) == 0;
    }

    if (column_type == TYPE_STRING) {
        /* Η VARCHAR συγκρίνεται μόνο με STRING_LITERAL. */
        return literal_types == TYPE_STRING;
    }

    return 0;
}

/*
 * Ελέγχει τη συμβατότητα του τύπου μιας στήλης
 * με ένα κυριολεκτικό ή μία λίστα κυριολεκτικών.
 */
void check_where_types(const char *table_name,
                       const char *column_name,
                       int literal_types)
{
    int column_type;

    column_type = get_column_type(table_name, column_name);

    /*
     * Αν η στήλη δεν υπάρχει, το αντίστοιχο μήνυμα
     * εμφανίζεται από τη check_column_reference().
     */
    if (column_type == 0) {
        return;
    }

    if (!types_are_compatible(column_type, literal_types)) {
        semantic_errors++;

        fprintf(stderr,
                "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                "η στήλη '%s' του πίνακα '%s' έχει τύπο %s "
                "και δεν είναι συμβατή με τα κυριολεκτικά "
                "της σύγκρισης.\n",
                line,
                column_name,
                table_name,
                type_to_string(column_type));
    }
}

/*
 * Καθαρίζει τη λίστα των πινάκων της τρέχουσας SELECT.
 */
void clear_available_tables(void)
{
    int i;

    for (i = 0; i < available_table_count; i++) {
        free(available_tables[i]);
        free(available_aliases[i]);
    }

    available_table_count = 0;
    current_join_table = NULL;
}

/*
 * Προσθέτει έναν πίνακα στους πίνακες που είναι διαθέσιμοι
 * στην τρέχουσα SELECT.
 */
void add_available_table(const char *table_name,
                         const char *alias)
{
    int i;

    /*
     * Το όνομα με το οποίο είναι ορατός ο πίνακας.
     * Αν έχει alias, χρησιμοποιείται το alias.
     */
    const char *visible_name =
        alias == NULL ? table_name : alias;

    if (available_table_count >= MAX_TABLES) {
        semantic_errors++;

        fprintf(stderr,
                "\nΣφάλμα: ξεπεράστηκε το μέγιστο πλήθος "
                "πινάκων στη SELECT.\n");

        return;
    }

    /*
     * Το πραγματικό όνομα του πίνακα πρέπει να έχει
     * δηλωθεί προηγουμένως με CREATE TABLE.
     */
    if (!table_exists(table_name)) {
        semantic_errors++;

        fprintf(stderr,
                "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                "ο πίνακας '%s' δεν έχει δημιουργηθεί "
                "προηγουμένως.\n",
                line, table_name);
    }

    /*
     * Δεν επιτρέπονται δύο ίδιοι ορατοί προσδιοριστές,
     * όπως δύο πίνακες με alias s.
     */
    for (i = 0; i < available_table_count; i++) {
        const char *other_visible_name;

        other_visible_name =
            available_aliases[i] == NULL
                ? available_tables[i]
                : available_aliases[i];

        if (strcmp(visible_name, other_visible_name) == 0) {
            semantic_errors++;

            fprintf(stderr,
                    "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                    "το όνομα ή alias '%s' χρησιμοποιείται "
                    "περισσότερες από μία φορές στο ίδιο query.\n",
                    line, visible_name);
        }
    }

    available_tables[available_table_count] =
        strdup(table_name);

    available_aliases[available_table_count] =
        alias == NULL ? NULL : strdup(alias);

    available_table_count++;
}


/*
 * Μετατρέπει ένα alias ή ένα όνομα πίνακα στο πραγματικό
 * όνομα του αντίστοιχου πίνακα.
 *
 * Παραδείγματα:
 *
 * students AS s:
 * resolve_qualifier("s", 1)        -> "students"
 * resolve_qualifier("students", 1) -> σημασιολογικό σφάλμα
 */
const char *resolve_qualifier(const char *qualifier,
                              int report_error)
{
    int i;

    /* Πρώτα αναζητούμε το qualifier στα aliases. */
    for (i = 0; i < available_table_count; i++) {
        if (available_aliases[i] != NULL &&
            strcmp(available_aliases[i], qualifier) == 0) {

            return available_tables[i];
        }
    }

    /*
     * Έπειτα εξετάζουμε αν είναι πραγματικό όνομα πίνακα.
     */
    for (i = 0; i < available_table_count; i++) {
        if (strcmp(available_tables[i], qualifier) == 0) {

            /*
             * Αν δεν έχει alias, επιτρέπεται να χρησιμοποιηθεί
             * το κανονικό όνομα του πίνακα.
             */
            if (available_aliases[i] == NULL) {
                return available_tables[i];
            }

            /*
             * Αν έχει alias, το πραγματικό όνομα δεν επιτρέπεται
             * πλέον ως πρόθεμα στήλης.
             */
            if (report_error) {
                semantic_errors++;

                fprintf(stderr,
                        "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                        "ο πίνακας '%s' έχει alias '%s'. "
                        "Πρέπει να χρησιμοποιηθεί "
                        "'%s.<στήλη>'.\n",
                        line,
                        available_tables[i],
                        available_aliases[i],
                        available_aliases[i]);
            }

            return NULL;
        }
    }

    if (report_error) {
        semantic_errors++;

        fprintf(stderr,
                "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                "το όνομα ή alias πίνακα '%s' δεν είναι "
                "διαθέσιμο στο συγκεκριμένο query.\n",
                line, qualifier);
    }

    return NULL;
}

/*
 * Επιστρέφει 1 αν ο πίνακας έχει εμφανιστεί:
 * - στο FROM,
 * - ή σε προηγούμενο JOIN.
 */
int table_is_available(const char *table_name)
{
    int i;

    for (i = 0; i < available_table_count; i++) {
        if (strcmp(available_tables[i], table_name) == 0) {
            return 1;
        }
    }

    return 0;
}

/*
 * Ελέγχει αν ένας πίνακας μπορεί να χρησιμοποιηθεί
 * στο ON του τρέχοντος JOIN.
 *
 * Επιτρέπεται:
 * - ένας πίνακας του FROM ή προηγούμενου JOIN,
 * - ο νέος πίνακας του τρέχοντος JOIN.
 */
int table_allowed_in_current_join(const char *table_name)
{
    if (table_is_available(table_name)) {
        return 1;
    }

    if (current_join_table != NULL &&
        strcmp(current_join_table, table_name) == 0) {
        return 1;
    }

    return 0;
}

/*
 * Ελέγχει έναν χαρακτηρισμένο προσδιορισμό:
 *
 *     όνομα_πίνακα.όνομα_στήλης
 *
 * μέσα στο ON.
 */
void check_join_column(const char *qualifier,
                       const char *column_name)
{
    const char *real_table;

    /*
     * Βρίσκουμε σε ποιον πραγματικό πίνακα αντιστοιχεί
     * το alias ή το όνομα πριν από την τελεία.
     */
    real_table = resolve_qualifier(qualifier, 1);

    if (real_table == NULL) {
        return;
    }

    if (!column_exists_in_table(real_table, column_name)) {
        semantic_errors++;

        fprintf(stderr,
                "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                "η στήλη '%s' δεν υπάρχει στον πίνακα '%s'.\n",
                line, column_name, real_table);
    }
}

/*
 * Επιστρέφει 1 αν μια μη χαρακτηρισμένη στήλη υπάρχει
 * σε τουλάχιστον έναν πίνακα της SELECT.
 */
int column_exists_in_available_tables(const char *column_name)
{
    int i;

    for (i = 0; i < available_table_count; i++) {
        if (column_exists_in_table(available_tables[i],
                                   column_name)) {
            return 1;
        }
    }

    return 0;
}


#line 858 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_CREATE = 3,                     /* CREATE  */
  YYSYMBOL_TABLE = 4,                      /* TABLE  */
  YYSYMBOL_SELECT = 5,                     /* SELECT  */
  YYSYMBOL_FROM = 6,                       /* FROM  */
  YYSYMBOL_WHERE = 7,                      /* WHERE  */
  YYSYMBOL_GROUP = 8,                      /* GROUP  */
  YYSYMBOL_BY = 9,                         /* BY  */
  YYSYMBOL_ORDER = 10,                     /* ORDER  */
  YYSYMBOL_LIMIT = 11,                     /* LIMIT  */
  YYSYMBOL_INT = 12,                       /* INT  */
  YYSYMBOL_FLOAT = 13,                     /* FLOAT  */
  YYSYMBOL_VARCHAR = 14,                   /* VARCHAR  */
  YYSYMBOL_AND = 15,                       /* AND  */
  YYSYMBOL_OR = 16,                        /* OR  */
  YYSYMBOL_NOT = 17,                       /* NOT  */
  YYSYMBOL_IN = 18,                        /* IN  */
  YYSYMBOL_IDENTIFIER = 19,                /* IDENTIFIER  */
  YYSYMBOL_INT_LITERAL = 20,               /* INT_LITERAL  */
  YYSYMBOL_FLOAT_LITERAL = 21,             /* FLOAT_LITERAL  */
  YYSYMBOL_STRING_LITERAL = 22,            /* STRING_LITERAL  */
  YYSYMBOL_EQ = 23,                        /* EQ  */
  YYSYMBOL_NE = 24,                        /* NE  */
  YYSYMBOL_LT = 25,                        /* LT  */
  YYSYMBOL_GT = 26,                        /* GT  */
  YYSYMBOL_LE = 27,                        /* LE  */
  YYSYMBOL_GE = 28,                        /* GE  */
  YYSYMBOL_JOIN = 29,                      /* JOIN  */
  YYSYMBOL_ON = 30,                        /* ON  */
  YYSYMBOL_AS = 31,                        /* AS  */
  YYSYMBOL_SEMICOLON = 32,                 /* SEMICOLON  */
  YYSYMBOL_33_ = 33,                       /* '('  */
  YYSYMBOL_34_ = 34,                       /* ')'  */
  YYSYMBOL_35_ = 35,                       /* ','  */
  YYSYMBOL_36_ = 36,                       /* '.'  */
  YYSYMBOL_37_ = 37,                       /* '*'  */
  YYSYMBOL_YYACCEPT = 38,                  /* $accept  */
  YYSYMBOL_program = 39,                   /* program  */
  YYSYMBOL_statements = 40,                /* statements  */
  YYSYMBOL_statement = 41,                 /* statement  */
  YYSYMBOL_create_start = 42,              /* create_start  */
  YYSYMBOL_create_table = 43,              /* create_table  */
  YYSYMBOL_table_name = 44,                /* table_name  */
  YYSYMBOL_column_list_table = 45,         /* column_list_table  */
  YYSYMBOL_column = 46,                    /* column  */
  YYSYMBOL_qualified_column = 47,          /* qualified_column  */
  YYSYMBOL_column_reference = 48,          /* column_reference  */
  YYSYMBOL_column_name = 49,               /* column_name  */
  YYSYMBOL_type = 50,                      /* type  */
  YYSYMBOL_select_start = 51,              /* select_start  */
  YYSYMBOL_table_source = 52,              /* table_source  */
  YYSYMBOL_select_table = 53,              /* select_table  */
  YYSYMBOL_join_list = 54,                 /* join_list  */
  YYSYMBOL_join_clause = 55,               /* join_clause  */
  YYSYMBOL_56_1 = 56,                      /* $@1  */
  YYSYMBOL_select_statement = 57,          /* select_statement  */
  YYSYMBOL_column_list = 58,               /* column_list  */
  YYSYMBOL_column_names = 59,              /* column_names  */
  YYSYMBOL_opt_where = 60,                 /* opt_where  */
  YYSYMBOL_opt_group_by = 61,              /* opt_group_by  */
  YYSYMBOL_opt_order_by = 62,              /* opt_order_by  */
  YYSYMBOL_opt_limit = 63,                 /* opt_limit  */
  YYSYMBOL_condition = 64,                 /* condition  */
  YYSYMBOL_predicate = 65,                 /* predicate  */
  YYSYMBOL_operator = 66,                  /* operator  */
  YYSYMBOL_value = 67,                     /* value  */
  YYSYMBOL_value_list = 68                 /* value_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   84

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  31
/* YYNRULES -- Number of rules.  */
#define YYNRULES  59
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  106

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   287


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      33,    34,    37,     2,    35,     2,    36,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   852,   852,   865,   866,   878,   879,   899,   905,   944,
     961,   962,   975,  1005,  1029,  1036,  1044,  1058,  1062,  1066,
    1096,  1116,  1123,  1137,  1150,  1152,  1165,  1164,  1193,  1232,
    1233,  1241,  1249,  1269,  1271,  1274,  1276,  1279,  1281,  1284,
    1286,  1307,  1308,  1309,  1310,  1311,  1330,  1338,  1346,  1362,
    1363,  1364,  1365,  1366,  1367,  1378,  1382,  1386,  1400,  1404
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "CREATE", "TABLE",
  "SELECT", "FROM", "WHERE", "GROUP", "BY", "ORDER", "LIMIT", "INT",
  "FLOAT", "VARCHAR", "AND", "OR", "NOT", "IN", "IDENTIFIER",
  "INT_LITERAL", "FLOAT_LITERAL", "STRING_LITERAL", "EQ", "NE", "LT", "GT",
  "LE", "GE", "JOIN", "ON", "AS", "SEMICOLON", "'('", "')'", "','", "'.'",
  "'*'", "$accept", "program", "statements", "statement", "create_start",
  "create_table", "table_name", "column_list_table", "column",
  "qualified_column", "column_reference", "column_name", "type",
  "select_start", "table_source", "select_table", "join_list",
  "join_clause", "$@1", "select_statement", "column_list", "column_names",
  "opt_where", "opt_group_by", "opt_order_by", "opt_limit", "condition",
  "predicate", "operator", "value", "value_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-80)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      39,     3,   -80,    17,    39,   -80,   -18,    11,     9,   -10,
     -80,   -80,   -80,   -80,   -80,   -80,    18,   -80,   -80,   -80,
      41,    20,    19,    34,     9,    37,    38,   -80,    27,   -80,
     -80,   -80,   -80,     0,   -80,    -2,    40,    -5,   -80,    38,
     -80,   -80,    28,   -80,   -80,    -4,     9,   -80,    52,   -80,
      42,    -4,    -4,    13,    30,   -80,   -80,    54,    56,    33,
     -80,   -12,    46,    35,   -80,   -80,   -80,   -80,   -80,   -80,
      -1,    -4,    -4,    43,    37,    60,    59,   -80,   -80,    44,
      -1,   -80,   -80,   -80,   -80,   -80,    57,    55,    20,    37,
      51,   -80,    -1,   -80,    14,    18,    53,    20,   -80,    16,
     -80,    -1,    55,   -80,   -80,   -80
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,    20,     0,     2,     3,     0,     0,     0,     0,
       1,     4,     5,     6,     9,     7,    14,    29,    15,    31,
       0,    30,     0,     0,     0,     0,     0,    13,    21,    23,
      24,    32,    16,     0,    10,     0,     0,    33,     8,     0,
      17,    18,     0,    12,    22,     0,     0,    25,    35,    11,
       0,     0,     0,     0,    34,    45,    26,     0,    37,     0,
      43,     0,     0,     0,    49,    50,    51,    52,    53,    54,
       0,     0,     0,     0,     0,     0,    39,    19,    44,     0,
       0,    55,    56,    57,    46,    41,    42,     0,    36,     0,
       0,    28,     0,    58,     0,     0,     0,    38,    40,     0,
      47,     0,     0,    48,    59,    27
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -80,   -80,   -80,    71,   -80,   -80,    70,   -80,    45,   -79,
      -9,   -80,   -80,   -80,    36,   -80,   -80,   -80,   -80,   -80,
     -80,   -56,   -80,   -80,   -80,   -80,   -46,   -80,   -80,   -69,
     -13
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     3,     4,     5,    22,     6,    28,    33,    34,    18,
      53,    35,    43,     9,    29,    30,    37,    47,    73,     7,
      20,    21,    48,    58,    76,    91,    54,    55,    70,    93,
      94
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      19,    84,    45,    71,    72,    60,    61,     8,    96,    16,
      40,    41,    42,    51,    12,    16,    31,    10,    88,    81,
      82,    83,    78,   105,    46,    85,    86,    17,    14,    52,
      62,    63,   104,    97,    38,    39,    64,    65,    66,    67,
      68,    69,     1,    13,     2,    71,    72,    24,   100,   101,
     103,   101,    26,    27,    23,    25,    16,    32,    36,    44,
      57,    50,    59,    74,    79,    19,    75,    77,    80,    89,
      90,    98,    71,    87,    95,    11,   102,    92,    15,    99,
      19,     0,    56,     0,    49
};

static const yytype_int8 yycheck[] =
{
       9,    70,     7,    15,    16,    51,    52,     4,    87,    19,
      12,    13,    14,    17,    32,    19,    25,     0,    74,    20,
      21,    22,    34,   102,    29,    71,    72,    37,    19,    33,
      17,    18,   101,    89,    34,    35,    23,    24,    25,    26,
      27,    28,     3,    32,     5,    15,    16,     6,    34,    35,
      34,    35,    33,    19,    36,    35,    19,    19,    31,    19,
       8,    33,    20,     9,    18,    74,    10,    34,    33,     9,
      11,    20,    15,    30,    19,     4,    23,    33,     8,    92,
      89,    -1,    46,    -1,    39
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     5,    39,    40,    41,    43,    57,     4,    51,
       0,    41,    32,    32,    19,    44,    19,    37,    47,    48,
      58,    59,    42,    36,     6,    35,    33,    19,    44,    52,
      53,    48,    19,    45,    46,    49,    31,    54,    34,    35,
      12,    13,    14,    50,    19,     7,    29,    55,    60,    46,
      33,    17,    33,    48,    64,    65,    52,     8,    61,    20,
      64,    64,    17,    18,    23,    24,    25,    26,    27,    28,
      66,    15,    16,    56,     9,    10,    62,    34,    34,    18,
      33,    20,    21,    22,    67,    64,    64,    30,    59,     9,
      11,    63,    33,    67,    68,    19,    47,    59,    20,    68,
      34,    35,    23,    34,    67,    47
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    38,    39,    40,    40,    41,    41,    42,    43,    44,
      45,    45,    46,    47,    48,    48,    49,    50,    50,    50,
      51,    52,    52,    53,    54,    54,    56,    55,    57,    58,
      58,    59,    59,    60,    60,    61,    61,    62,    62,    63,
      63,    64,    64,    64,    64,    64,    65,    65,    65,    66,
      66,    66,    66,    66,    66,    67,    67,    67,    68,    68
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     2,     2,     0,     7,     1,
       1,     3,     2,     3,     1,     1,     1,     1,     1,     4,
       0,     1,     3,     1,     0,     2,     0,     7,    10,     1,
       1,     1,     3,     0,     2,     0,     3,     0,     3,     0,
       2,     3,     3,     2,     3,     1,     3,     5,     6,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 7: /* create_start: %empty  */
#line 899 "parser.y"
    {
        clear_current_columns();
    }
#line 1981 "parser.tab.c"
    break;

  case 8: /* create_table: CREATE TABLE table_name create_start '(' column_list_table ')'  */
#line 907 "parser.y"
    {
        if (table_exists((yyvsp[-4].str))) {
            semantic_errors++;

            fprintf(stderr,
                    "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                    "ο πίνακας '%s' έχει ήδη δημιουργηθεί.\n",
                    line, (yyvsp[-4].str));
        }
        else if (semantic_errors == create_start_errors) {
            /*
             * Δεν εντοπίστηκε κανένα σημασιολογικό σφάλμα
             * κατά την ανάλυση των στηλών.
             */
            add_table((yyvsp[-4].str));

            printf("\nΟ πίνακας '%s' δημιουργήθηκε επιτυχώς.\n",
                   (yyvsp[-4].str));
        }
        else {
            /*
             * Η σύνταξη της CREATE TABLE είναι σωστή,
             * αλλά υπάρχει σημασιολογικό σφάλμα στις στήλες.
             */
            fprintf(stderr,
                    "\nΗ δημιουργία του πίνακα '%s' "
                    "δεν ολοκληρώθηκε λόγω σημασιολογικών σφαλμάτων.\n",
                    (yyvsp[-4].str));
        }

        clear_current_columns();
        free((yyvsp[-4].str));
    }
#line 2019 "parser.tab.c"
    break;

  case 9: /* table_name: IDENTIFIER  */
#line 945 "parser.y"
    {
        (yyval.str) = (yyvsp[0].str); /* Το $1 είναι η τιμή του IDENTIFIER, ενώ το $$ είναι η τιμή που επιστρέφει το table_name. */
    }
#line 2027 "parser.tab.c"
    break;

  case 12: /* column: column_name type  */
#line 976 "parser.y"
    {
        /*
         * Εδώ γνωρίζουμε ταυτόχρονα το όνομα της στήλης
         * και τον τύπο δεδομένων της.
         */
        if (current_column_exists((yyvsp[-1].str))) {
            semantic_errors++;

            fprintf(stderr,
                    "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                    "η στήλη '%s' έχει δηλωθεί περισσότερες "
                    "από μία φορές στον ίδιο πίνακα.\n",
                    line, (yyvsp[-1].str));
        }
        else {
            add_current_column((yyvsp[-1].str), (yyvsp[0].data_type));
        }

        free((yyvsp[-1].str));
    }
#line 2052 "parser.tab.c"
    break;

  case 13: /* qualified_column: IDENTIFIER '.' IDENTIFIER  */
#line 1006 "parser.y"
    {
        (yyval.names) = malloc(2 * sizeof(char *));

        if ((yyval.names) == NULL) {
            fprintf(stderr,
                    "\nΣφάλμα: αποτυχία δέσμευσης μνήμης.\n");

            exit(EXIT_FAILURE);
        }

        (yyval.names)[0] = (yyvsp[-2].str);
        (yyval.names)[1] = (yyvsp[0].str);
    }
#line 2070 "parser.tab.c"
    break;

  case 14: /* column_reference: IDENTIFIER  */
#line 1030 "parser.y"
    {
        (yyval.names) = malloc(2 * sizeof(char *));

        (yyval.names)[0] = NULL;
        (yyval.names)[1] = (yyvsp[0].str);
    }
#line 2081 "parser.tab.c"
    break;

  case 15: /* column_reference: qualified_column  */
#line 1037 "parser.y"
    {
        (yyval.names) = (yyvsp[0].names);
    }
#line 2089 "parser.tab.c"
    break;

  case 16: /* column_name: IDENTIFIER  */
#line 1045 "parser.y"
    {
        (yyval.str) = (yyvsp[0].str);
    }
#line 2097 "parser.tab.c"
    break;

  case 17: /* type: INT  */
#line 1059 "parser.y"
    {
        (yyval.data_type) = TYPE_INT;
    }
#line 2105 "parser.tab.c"
    break;

  case 18: /* type: FLOAT  */
#line 1063 "parser.y"
    {
        (yyval.data_type) = TYPE_FLOAT;
    }
#line 2113 "parser.tab.c"
    break;

  case 19: /* type: VARCHAR '(' INT_LITERAL ')'  */
#line 1067 "parser.y"
    {
        /*
         * Για τον συγκεκριμένο σημασιολογικό έλεγχο
         * αρκεί να θυμόμαστε ότι είναι VARCHAR.
         */
        (yyval.data_type) = TYPE_STRING;
    }
#line 2125 "parser.tab.c"
    break;

  case 20: /* select_start: %empty  */
#line 1096 "parser.y"
    {
        clear_used_columns();
        clear_available_tables();
        current_select_table = NULL;
        current_join_table = NULL;
    }
#line 2136 "parser.tab.c"
    break;

  case 21: /* table_source: table_name  */
#line 1117 "parser.y"
    {
        (yyval.names) = malloc(2 * sizeof(char *));

        (yyval.names)[0] = (yyvsp[0].str);
        (yyval.names)[1] = NULL;
    }
#line 2147 "parser.tab.c"
    break;

  case 22: /* table_source: table_name AS IDENTIFIER  */
#line 1124 "parser.y"
    {
        (yyval.names) = malloc(2 * sizeof(char *));

        (yyval.names)[0] = (yyvsp[-2].str);
        (yyval.names)[1] = (yyvsp[0].str);
    }
#line 2158 "parser.tab.c"
    break;

  case 23: /* select_table: table_source  */
#line 1138 "parser.y"
    {
        add_available_table((yyvsp[0].names)[0], (yyvsp[0].names)[1]);
        (yyval.names) = (yyvsp[0].names);
    }
#line 2167 "parser.tab.c"
    break;

  case 26: /* $@1: %empty  */
#line 1165 "parser.y"
    {
        /*
         * Ο πίνακας του JOIN προστίθεται πριν από το ON,
         * ώστε το alias του να είναι ήδη διαθέσιμο κατά
         * τον σημασιολογικό έλεγχο του ON.
         */
        add_available_table((yyvsp[0].names)[0], (yyvsp[0].names)[1]);
    }
#line 2180 "parser.tab.c"
    break;

  case 27: /* join_clause: JOIN table_source $@1 ON qualified_column EQ qualified_column  */
#line 1174 "parser.y"
    {
        check_join_column((yyvsp[-2].names)[0], (yyvsp[-2].names)[1]);
        check_join_column((yyvsp[0].names)[0], (yyvsp[0].names)[1]);

        free((yyvsp[-2].names)[0]);
        free((yyvsp[-2].names)[1]);
        free((yyvsp[-2].names));

        free((yyvsp[0].names)[0]);
        free((yyvsp[0].names)[1]);
        free((yyvsp[0].names));

        free((yyvsp[-5].names)[0]);
        free((yyvsp[-5].names)[1]);
        free((yyvsp[-5].names));
    }
#line 2201 "parser.tab.c"
    break;

  case 28: /* select_statement: SELECT select_start column_list FROM select_table join_list opt_where opt_group_by opt_order_by opt_limit  */
#line 1200 "parser.y"
    {
        /*
         * Ο έλεγχος γίνεται στο τέλος επειδή το SELECT
         * αναλύεται πριν από το FROM. Μόνο εδώ γνωρίζουμε
         * όλους τους πίνακες και όλα τα aliases.
         */
        check_used_columns();

        clear_used_columns();
        clear_available_tables();

        current_select_table = NULL;
        current_join_table = NULL;

        free((yyvsp[-5].names)[0]);
        free((yyvsp[-5].names)[1]);
        free((yyvsp[-5].names));
    }
#line 2224 "parser.tab.c"
    break;

  case 31: /* column_names: column_reference  */
#line 1242 "parser.y"
    {
        add_used_column((yyvsp[0].names)[0], (yyvsp[0].names)[1], 0);

        free((yyvsp[0].names)[0]);
        free((yyvsp[0].names)[1]);
        free((yyvsp[0].names));
    }
#line 2236 "parser.tab.c"
    break;

  case 32: /* column_names: column_names ',' column_reference  */
#line 1250 "parser.y"
    {
        add_used_column((yyvsp[0].names)[0], (yyvsp[0].names)[1], 0);

        free((yyvsp[0].names)[0]);
        free((yyvsp[0].names)[1]);
        free((yyvsp[0].names));
    }
#line 2248 "parser.tab.c"
    break;

  case 46: /* predicate: column_reference operator value  */
#line 1331 "parser.y"
    {
        add_used_column((yyvsp[-2].names)[0], (yyvsp[-2].names)[1], (yyvsp[0].data_type));

        free((yyvsp[-2].names)[0]);
        free((yyvsp[-2].names)[1]);
        free((yyvsp[-2].names));
    }
#line 2260 "parser.tab.c"
    break;

  case 47: /* predicate: column_reference IN '(' value_list ')'  */
#line 1339 "parser.y"
    {
        add_used_column((yyvsp[-4].names)[0], (yyvsp[-4].names)[1], (yyvsp[-1].data_type));

        free((yyvsp[-4].names)[0]);
        free((yyvsp[-4].names)[1]);
        free((yyvsp[-4].names));
    }
#line 2272 "parser.tab.c"
    break;

  case 48: /* predicate: column_reference NOT IN '(' value_list ')'  */
#line 1347 "parser.y"
    {
        add_used_column((yyvsp[-5].names)[0], (yyvsp[-5].names)[1], (yyvsp[-1].data_type));

        free((yyvsp[-5].names)[0]);
        free((yyvsp[-5].names)[1]);
        free((yyvsp[-5].names));
    }
#line 2284 "parser.tab.c"
    break;

  case 55: /* value: INT_LITERAL  */
#line 1379 "parser.y"
    {
        (yyval.data_type) = TYPE_INT;
    }
#line 2292 "parser.tab.c"
    break;

  case 56: /* value: FLOAT_LITERAL  */
#line 1383 "parser.y"
    {
        (yyval.data_type) = TYPE_FLOAT;
    }
#line 2300 "parser.tab.c"
    break;

  case 57: /* value: STRING_LITERAL  */
#line 1387 "parser.y"
    {
        (yyval.data_type) = TYPE_STRING;
    }
#line 2308 "parser.tab.c"
    break;

  case 58: /* value_list: value  */
#line 1401 "parser.y"
    {
        (yyval.data_type) = (yyvsp[0].data_type);
    }
#line 2316 "parser.tab.c"
    break;

  case 59: /* value_list: value_list ',' value  */
#line 1405 "parser.y"
    {
        /*
         * Συνδυάζουμε τους τύπους όλων των κυριολεκτικών.
         *
         * Για παράδειγμα:
         * (10, 12.5) → TYPE_INT | TYPE_FLOAT
         */
        (yyval.data_type) = (yyvsp[-2].data_type) | (yyvsp[0].data_type);
    }
#line 2330 "parser.tab.c"
    break;


#line 2334 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1420 "parser.y"


/*
 * Η yyerror() καλείται αυτόματα από τον Bison όταν η σειρά
 * των tokens δεν συμφωνεί με κανέναν κανόνα της γραμματικής.
 *
 * Η παράμετρος s περιέχει συνήθως το μήνυμα "syntax error".
 * Εδώ δεν τη χρησιμοποιούμε και εμφανίζουμε δικό μας μήνυμα.
 *
 * Η μεταβλητή line μάς δείχνει τη γραμμή στην οποία
 * εντοπίστηκε το συντακτικό λάθος.
*/
void yyerror(const char *s) {
    printf("\nSyntax error at line %d\n", line);

    
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: myParser.exe file_name\n");
        return 1;
    }

    yyin = fopen(argv[1], "r");

    if (!yyin) {
        printf("Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    
    
    
int result = yyparse();

if (result == 0) {
    if (semantic_errors == 0) {
        printf("\nΤο πρόγραμμα είναι συντακτικά και σημασιολογικά ορθό.\n");
    } else {
        printf("\nΗ συντακτική ανάλυση ολοκληρώθηκε επιτυχώς, "
               "αλλά εντοπίστηκαν %d σημασιολογικά σφάλματα.\n",
               semantic_errors);
    }
} else {
    printf("\nΤο πρόγραμμα περιέχει συντακτικά σφάλματα.\n");
}
    fclose(yyin);
    return 0;
}
