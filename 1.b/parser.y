%{
#include <stdio.h>
#include <stdlib.h>

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


/* Συνάρτηση που καλείται όταν ο Bison εντοπίσει συντακτικό λάθος. */
void yyerror(const char *s);
%}

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
%token IDENTIFIER INT_LITERAL FLOAT_LITERAL STRING_LITERAL
%token EQ NE LT GT LE GE

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
create_table:
    CREATE TABLE table_name '(' column_list_table ')'
    ;

/* Το όνομα του πίνακα πρέπει να είναι ένα αναγνωριστικό. */
table_name:
    IDENTIFIER
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

select_statement:
    SELECT column_list FROM table_name opt_where opt_group_by opt_order_by opt_limit
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
    | column_names ',' IDENTIFIER
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
    | IDENTIFIER IN '(' value_list ')'
    | IDENTIFIER NOT IN '(' value_list ')'
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

    /* Τερματίζουμε το πρόγραμμα με κωδικό αποτυχίας. */
    exit(1);
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
    
    
    /*
     * Ξεκινά τη συντακτική ανάλυση.
     *
     * Η yyparse() δημιουργείται αυτόματα από τον Bison.
     * Κατά την εκτέλεσή της καλεί τη yylex(), η οποία
     * επιστρέφει τα tokens από τον Flex.
    */
    yyparse();
    

    /*
     * Αν η yyparse() ολοκληρωθεί χωρίς να κληθεί η yyerror(),
     * τότε το αρχείο είναι συντακτικά σωστό.
    */
    printf("\nProgram is syntactically correct.\n");

    fclose(yyin);
    return 0;
}