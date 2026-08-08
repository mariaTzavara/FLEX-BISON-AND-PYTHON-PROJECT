%{
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

%}


%union {
    char *str;
    char **names;
    int data_type;
}

/* =========================================================
 *                       ΔΗΛΩΣΗ TOKENS
 * =========================================================
 *
 * Τα tokens είναι οι κατηγορίες λεκτικών στοιχείων που
 * αναγνωρίζει ο Flex και επιστρέφει στον Bison.
*/

%token CREATE TABLE SELECT FROM WHERE GROUP BY ORDER LIMIT
%token INT FLOAT VARCHAR
%token AND OR NOT IN
%token <str> IDENTIFIER 
%token INT_LITERAL FLOAT_LITERAL STRING_LITERAL
%token EQ NE LT GT LE GE
%token JOIN ON AS

%type <str> table_name column_name
%type <data_type> type value value_list
%type <names> qualified_column column_reference
%type <names> table_source select_table

/* Το ελληνικό ερωτηματικό/semicolon ολοκληρώνει μια εντολή SQL. */
%token SEMICOLON

/* =========================================================
 *                 ΠΡΟΤΕΡΑΙΟΤΗΤΑ ΤΕΛΕΣΤΩΝ
 * =========================================================
 *
 * Όσο χαμηλότερα δηλώνεται ένας τελεστής, τόσο μεγαλύτερη
 * προτεραιότητα έχει.
 *
 * Άρα η σειρά προτεραιότητας είναι:
 *
 *     NOT > AND > OR
*/
%left OR
%left AND
%right NOT


/*
 * Το πρώτο %% δηλώνει την αρχή των κανόνων της γραμματικής.
*/
%%


/* =========================================================
 *                    ΑΡΧΙΚΟΣ ΚΑΝΟΝΑΣ
 * ========================================================= 
*/

/*
 * Το program είναι το αρχικό σύμβολο της γραμματικής.
 * Ένα πρόγραμμα αποτελείται από μία ή περισσότερες
 * εντολές SQL.
*/
program:
    statements
    ;

/*
 * Τα statements μπορούν να αποτελούνται:
 *
 * 1. Από μία μόνο εντολή.
 * 2. Από μια ήδη υπάρχουσα λίστα εντολών και μία νέα εντολή.
 *
 * Ο δεύτερος κανόνας είναι αριστερά αναδρομικός και επιτρέπει
 * την αναγνώριση πολλών διαδοχικών εντολών.
*/
statements:
    statement
    | statements statement
    ;

/*
 * Κάθε statement μπορεί να είναι:
 *
 * 1. Μια εντολή CREATE TABLE.
 * 2. Μια εντολή SELECT.
 *
 * Και στις δύο περιπτώσεις πρέπει να ολοκληρώνεται με semicolon.
*/
statement:
    create_table SEMICOLON
    | select_statement SEMICOLON
    ;

/* =========================================================
 *                     ΕΝΤΟΛΗ CREATE TABLE
 * ========================================================= 
*/

/*
 * Η εντολή CREATE TABLE έχει τη μορφή:
 *
 * CREATE TABLE όνομα_πίνακα (λίστα_στηλών)
 *
 * Παράδειγμα:
 *
 * CREATE TABLE students (name VARCHAR(20), age INT);
*/

create_start:
    /* empty */
    {
        clear_current_columns();
    }
    ;

create_table:
    CREATE TABLE table_name create_start
    '(' column_list_table ')'
    {
        if (table_exists($3)) {
            semantic_errors++;

            fprintf(stderr,
                    "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                    "ο πίνακας '%s' έχει ήδη δημιουργηθεί.\n",
                    line, $3);
        }
        else {
            add_table($3);

            printf("\nΟ πίνακας '%s' δημιουργήθηκε επιτυχώς.\n",
                   $3);
        }

        clear_current_columns();
        free($3);
    }
    ;

/* Το όνομα του πίνακα πρέπει να είναι ένα αναγνωριστικό. */
table_name:
    IDENTIFIER
    {
        $$ = $1; /* Το $1 είναι η τιμή του IDENTIFIER, ενώ το $$ είναι η τιμή που επιστρέφει το table_name. */
    }
    ;

/*
 * Η λίστα των στηλών μπορεί να περιέχει:
 *
 * 1. Μία μόνο στήλη.
 * 2. Πολλές στήλες χωρισμένες με κόμμα.
 *
 * Παράδειγμα:
 *
 * name VARCHAR(20), age INT, grade FLOAT
*/
column_list_table:
    column
    | column_list_table ',' column
    ;

/*
 * Κάθε στήλη αποτελείται από:
 *
 *     όνομα_στήλης τύπος
 *
 * Παράδειγμα:
 *
 *     age INT
*/
column:
    column_name type
    {
        /*
         * Εδώ γνωρίζουμε ταυτόχρονα το όνομα της στήλης
         * και τον τύπο δεδομένων της.
         */
        if (current_column_exists($1)) {
            semantic_errors++;

            fprintf(stderr,
                    "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                    "η στήλη '%s' έχει δηλωθεί περισσότερες "
                    "από μία φορές στον ίδιο πίνακα.\n",
                    line, $1);
        }
        else {
            add_current_column($1, $2);
        }

        free($1);
    }
    ;

/*
 * Αναγνωρίζει:
 *
 * alias.column
 * table.column
 */
qualified_column:
    IDENTIFIER '.' IDENTIFIER
    {
        $$ = malloc(2 * sizeof(char *));

        if ($$ == NULL) {
            fprintf(stderr,
                    "\nΣφάλμα: αποτυχία δέσμευσης μνήμης.\n");

            exit(EXIT_FAILURE);
        }

        $$[0] = $1;
        $$[1] = $3;
    }
    ;
    
/*
 * Μία αναφορά στήλης μπορεί να είναι:
 *
 * name
 * s.name
 * students.name
 */
column_reference:
    IDENTIFIER
    {
        $$ = malloc(2 * sizeof(char *));

        $$[0] = NULL;
        $$[1] = $1;
    }
    | qualified_column
    {
        $$ = $1;
    }
    ;

/* Το όνομα μιας στήλης είναι ένα αναγνωριστικό. */
column_name:
    IDENTIFIER
    {
        $$ = $1;
    }
    ;

/*
 * Οι τύποι δεδομένων που υποστηρίζει η γραμματική είναι:
 *
 *     INT
 *     FLOAT
 *     VARCHAR(ακέραιο_μήκος)
*/
type:
    INT
    {
        $$ = TYPE_INT;
    }
    | FLOAT
    {
        $$ = TYPE_FLOAT;
    }
    | VARCHAR '(' INT_LITERAL ')'
    {
        /*
         * Για τον συγκεκριμένο σημασιολογικό έλεγχο
         * αρκεί να θυμόμαστε ότι είναι VARCHAR.
         */
        $$ = TYPE_STRING;
    }
    ;

/* =========================================================
 *                        ΕΝΤΟΛΗ SELECT
 * ========================================================= 
*/

/*
 * Η βασική μορφή μιας SELECT είναι:
 *
 * SELECT στήλες
 * FROM πίνακας
 * [WHERE συνθήκη]
 * [GROUP BY στήλες]
 * [ORDER BY στήλες]
 * [LIMIT ακέραιος]
 *
 * Τα τμήματα μέσα σε αγκύλες είναι προαιρετικά.
*/

select_start:
    /* empty */
    {
        clear_used_columns();
        clear_available_tables();
        current_select_table = NULL;
        current_join_table = NULL;
    }
    ;

/*
 * Ένας πίνακας μπορεί να εμφανιστεί:
 *
 * students
 * students AS s
 *
 * Επιστρέφεται πίνακας δύο συμβολοσειρών:
 *
 * $$[0] = πραγματικό όνομα πίνακα
 * $$[1] = alias ή NULL
 */
table_source:
    table_name
    {
        $$ = malloc(2 * sizeof(char *));

        $$[0] = $1;
        $$[1] = NULL;
    }
    | table_name AS IDENTIFIER
    {
        $$ = malloc(2 * sizeof(char *));

        $$[0] = $1;
        $$[1] = $3;
    }
    ;

/*
 * Αποθηκεύει προσωρινά τον πίνακα του FROM, ώστε οι κανόνες
 * του WHERE να μπορούν να βρουν τους τύπους των στηλών του.
 */
select_table:
    table_source
    {
        add_available_table($1[0], $1[1]);
        $$ = $1;
    }
    ;

/*
 * Ο όρος JOIN μπορεί να εμφανιστεί:
 * - μηδέν φορές,
 * - μία φορά,
 * - πολλές φορές.
 */
join_list:
    /* empty */
    | join_list join_clause
    ;



/*
 * Γραμματική ενός JOIN:
 *
 * JOIN table2
 * ON table1.column1 = table2.column2
 */
join_clause:
    JOIN table_source
    {
        /*
         * Ο πίνακας του JOIN προστίθεται πριν από το ON,
         * ώστε το alias του να είναι ήδη διαθέσιμο κατά
         * τον σημασιολογικό έλεγχο του ON.
         */
        add_available_table($2[0], $2[1]);
    }
    ON qualified_column EQ qualified_column
    {
        check_join_column($5[0], $5[1]);
        check_join_column($7[0], $7[1]);

        free($5[0]);
        free($5[1]);
        free($5);

        free($7[0]);
        free($7[1]);
        free($7);

        free($2[0]);
        free($2[1]);
        free($2);
    }
    ;

select_statement:
    SELECT select_start column_list
    FROM select_table
    join_list
    opt_where
    opt_group_by
    opt_order_by
    opt_limit
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

        free($5[0]);
        free($5[1]);
        free($5);
    }
    ;
    
/*
 * Μετά το SELECT μπορούμε να έχουμε:
 *
 * 1. Τον χαρακτήρα *, δηλαδή όλες τις στήλες.
 * 2. Μια λίστα συγκεκριμένων ονομάτων στηλών.
 *
 * Παραδείγματα:
 *
 * SELECT * FROM students;
 * SELECT name, age FROM students;
*/
column_list:
    '*'
    | column_names
    ;

/*
 * Μια λίστα ονομάτων στηλών περιέχει μία ή περισσότερες
 * στήλες, χωρισμένες με κόμμα.
*/
column_names:
    column_reference
    {
        add_used_column($1[0], $1[1], 0);

        free($1[0]);
        free($1[1]);
        free($1);
    }
    | column_names ',' column_reference
    {
        add_used_column($3[0], $3[1], 0);

        free($3[0]);
        free($3[1]);
        free($3);
    }
    ;


/* =========================================================
 *                     ΠΡΟΑΙΡΕΤΙΚΕΣ ΦΡΑΣΕΙΣ
 * =========================================================
 *
 * Το πρόθεμα opt σημαίνει optional, δηλαδή προαιρετικό.
 * Ο κενός κανόνας επιτρέπει να μην υπάρχει η συγκεκριμένη
 * φράση μέσα στην εντολή SELECT.
*/

opt_where:
    /* empty */
    | WHERE condition
    ;

opt_group_by:
    /* empty */
    | GROUP BY column_names
    ;

opt_order_by:
    /* empty */
    | ORDER BY column_names
    ;

opt_limit:
    /* empty */
    | LIMIT INT_LITERAL
    ;


/* =========================================================
 *                         ΣΥΝΘΗΚΕΣ
 * ========================================================= */

/*
 * Μια συνθήκη μπορεί να αποτελείται από:
 *
 * 1. Δύο συνθήκες ενωμένες με AND.
 * 2. Δύο συνθήκες ενωμένες με OR.
 * 3. Άρνηση μιας συνθήκης με NOT.
 * 4. Ένα απλό predicate.
 *
 * Παράδειγμα:
 *
 * age >= 18 AND grade > 5
*/
condition:
    condition AND condition
    | condition OR condition
    | NOT condition
    | '(' condition ')'
    | predicate
    ;

/*
 * Ένα predicate είναι μια βασική συνθήκη και μπορεί να έχει
 * μία από τις παρακάτω μορφές:
 *
 * 1. στήλη τελεστής τιμή
 * 2. στήλη IN (λίστα τιμών)
 * 3. στήλη NOT IN (λίστα τιμών)
 *
 * Παραδείγματα:
 *
 *     age >= 18
 *     grade = 10
 *     city IN ("Athens", "Patras")
 *     age NOT IN (15, 16, 17)
*/
predicate:
    column_reference operator value
    {
        add_used_column($1[0], $1[1], $3);

        free($1[0]);
        free($1[1]);
        free($1);
    }
    | column_reference IN '(' value_list ')'
    {
        add_used_column($1[0], $1[1], $4);

        free($1[0]);
        free($1[1]);
        free($1);
    }
    | column_reference NOT IN '(' value_list ')'
    {
        add_used_column($1[0], $1[1], $5);

        free($1[0]);
        free($1[1]);
        free($1);
    }
    ;

/*
 * Υποστηριζόμενοι τελεστές σύγκρισης:
 *
 *     =   !=   <   >   <=   >=
*/
operator:
    EQ
    | NE
    | LT
    | GT
    | LE
    | GE
    ;

/*
 * Μια τιμή μπορεί να είναι:
 *
 * 1. Ακέραιος αριθμός.
 * 2. Πραγματικός αριθμός.
 * 3. Συμβολοσειρά.
*/
value:
    INT_LITERAL
    {
        $$ = TYPE_INT;
    }
    | FLOAT_LITERAL
    {
        $$ = TYPE_FLOAT;
    }
    | STRING_LITERAL
    {
        $$ = TYPE_STRING;
    }
    ;

/*
 * Η λίστα τιμών χρησιμοποιείται μέσα στις εκφράσεις IN
 * και NOT IN.
 *
 * Μπορεί να περιέχει μία ή περισσότερες τιμές, χωρισμένες
 * με κόμμα.
*/
value_list:
    value
    {
        $$ = $1;
    }
    | value_list ',' value
    {
        /*
         * Συνδυάζουμε τους τύπους όλων των κυριολεκτικών.
         *
         * Για παράδειγμα:
         * (10, 12.5) → TYPE_INT | TYPE_FLOAT
         */
        $$ = $1 | $3;
    }
    ;

/*
 * Το δεύτερο %% δηλώνει το τέλος της γραμματικής.
 * Από αυτό το σημείο και μετά γράφουμε κανονικό κώδικα C.
*/
%%

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
