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

/*
 * Κάθε πίνακας έχει:
 *
 * 1. Το δικό του όνομα.
 * 2. Έναν πίνακα με τα ονόματα των στηλών του.
 * 3. Το πλήθος των στηλών του.
 */
typedef struct {
    char *name;
    char *columns[MAX_COLUMNS];
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
int current_column_count = 0;

/*
 * Προσωρινή λίστα των στηλών που χρησιμοποιούνται
 * στην SELECT που αναλύεται αυτή τη στιγμή.
 */
char *used_columns[MAX_COLUMNS];
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

/* Προσθέτει μια στήλη στην προσωρινή λίστα της CREATE TABLE. */
void add_current_column(const char *name)
{
    if (current_column_count >= MAX_COLUMNS) {
        fprintf(stderr,
                "Σφάλμα: ξεπεράστηκε το μέγιστο πλήθος στηλών.\n");
        semantic_errors++;
        return;
    }

    current_columns[current_column_count] = strdup(name);
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

/* Αποθηκεύει προσωρινά μια στήλη που χρησιμοποιεί η SELECT. */
void add_used_column(const char *name)
{
    if (used_column_count >= MAX_COLUMNS) {
        fprintf(stderr,
                "Σφάλμα: ξεπεράστηκε το μέγιστο πλήθος "
                "στηλών στη SELECT.\n");
        semantic_errors++;
        return;
    }

    used_columns[used_column_count] = strdup(name);
    used_column_count++;
}

/* Καθαρίζει τις προσωρινές στήλες της SELECT. */
void clear_used_columns(void)
{
    int i;

    for (i = 0; i < used_column_count; i++) {
        free(used_columns[i]);
    }

    used_column_count = 0;
}

/*
 * Ελέγχει ότι όλες οι στήλες της SELECT έχουν δηλωθεί
 * στην CREATE TABLE του πίνακα που βρίσκεται στο FROM.
 */
void check_used_columns(const char *table_name)
{
    int i;

    for (i = 0; i < used_column_count; i++) {
        if (!column_exists_in_table(table_name, used_columns[i])) {
            semantic_errors++;

            fprintf(stderr,
                    "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                    "η στήλη '%s' δεν έχει οριστεί στον πίνακα '%s'.\n",
                    line,
                    used_columns[i],
                    table_name);
        }
    }
}



%}


%union {
    char *str;
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

%type <str> table_name

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
    ;

/* Το όνομα μιας στήλης είναι ένα αναγνωριστικό. */
column_name:
    IDENTIFIER
    {
        /*
         * Ο έλεγχος γίνεται μόνο στην προσωρινή λίστα του
         * συγκεκριμένου CREATE TABLE.
         *
         * Επομένως, το ίδιο όνομα επιτρέπεται σε διαφορετικούς
         * πίνακες αλλά όχι δύο φορές στον ίδιο πίνακα.
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
            add_current_column($1);
        }

        free($1);
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
    | FLOAT
    | VARCHAR '(' INT_LITERAL ')'
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
    }
    ;

select_statement:
    SELECT select_start column_list FROM table_name
    opt_where opt_group_by opt_order_by opt_limit
    {
        
        if (!table_exists($5)) {
            semantic_errors++;

            fprintf(stderr,
                    "\nΣημασιολογικό σφάλμα στη γραμμή %d: "
                    "ο πίνακας '%s' που χρησιμοποιείται στον όρο "
                    "FROM δεν έχει δημιουργηθεί προηγουμένως.\n",
                    line, $5);
        }
        else {
            /*
             * Αν ο πίνακας υπάρχει, ελέγχουμε ότι όλες οι
             * χρησιμοποιούμενες στήλες ανήκουν σε αυτόν.
             */
            check_used_columns($5);
        }

        clear_used_columns();
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
    IDENTIFIER
    {
        add_used_column($1);
        free($1);
    }
    | column_names ',' IDENTIFIER
    {
        add_used_column($3);
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
    IDENTIFIER operator value
    {
        add_used_column($1);
        free($1);
    }
    | IDENTIFIER IN '(' value_list ')'
    {
        add_used_column($1);
        free($1);
    }
    | IDENTIFIER NOT IN '(' value_list ')'
    {
        add_used_column($1);
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
    | FLOAT_LITERAL
    | STRING_LITERAL
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
    | value_list ',' value
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
