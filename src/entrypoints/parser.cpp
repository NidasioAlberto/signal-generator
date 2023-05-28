#include <miosix.h>
#include <parser/lexer.h>
#include <parser/parser.h>

using namespace miosix;

void yyerror(char *message) { printf("Error: %s\n", message); }

int main() {
    while (1) {
        fflush(stdin);
        char expr[256];

        if (!fgets(expr, sizeof(expr), stdin)) {
            continue;
        }

        YY_BUFFER_STATE state;

        if (!(state = yy_scan_bytes(expr, strcspn(expr, "\n")))) {
            continue;
        }

        yyparse();
        yy_delete_buffer(state);
    }
}