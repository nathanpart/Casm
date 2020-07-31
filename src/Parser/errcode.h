#ifndef ERRCODE_H
#define ERRCODE_H


/* Error codes passed around between file in_stream, sourceTokenizer, parser and
   interpreter.  This is necessary so we can turn them into Python
   exceptions at a higher level.  Note that some errors have a
   slightly different meaning when passed from the sourceTokenizer to the
   parser than when passed from the parser to the interpreter; e.g.
   the parser only returns E_EOF when it hits EOF immediately, and it
   never returns E_OK. */

#define E_OK            10      /* No error */
#define E_EOF           11      /* End Of File */
#define E_INTR          12      /* Interrupted */
#define E_TOKEN         13      /* Bad token */
#define E_SYNTAX        14      /* Syntax error */
#define E_NOMEM         15      /* Ran out of memory */
#define E_DONE          16      /* Parsing complete */
#define E_ERROR         17      /* Execution error */
#define E_EOL           18      /* Hit end of line while searching for the closing delimiter */
#define E_OVERFLOW      19      /* Node had too many children */
#define E_IDENTIFIER    26      /* Invalid characters in identifier */
#define E_STREAM        28      /* Error while fetching line from input stream */

#endif /* !Py_ERRCODE_H */
