
/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, říjen 2017                                        */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického
** výrazu do postfixového tvaru. Pro převod využijte zásobník (tStack),
** který byl implementován v rámci příkladu c202. Bez správného vyřešení
** příkladu c202 se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix .... konverzní funkce pro převod infixového výrazu 
**                       na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**    
**    untilLeftPar .... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

int solved;


/*
** Pomocná funkce untilLeftPar.
** Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka
** bude také odstraněna. Pokud je zásobník prázdný, provádění funkce se ukončí.
**
** Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
** znaků postExpr. Délka převedeného výrazu a též ukazatel na první volné
** místo, na které se má zapisovat, představuje parametr postLen.
**
** Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
** nadeklarovat a používat pomocnou proměnnou typu char.
*/
void untilLeftPar ( tStack* s, char* postExpr, unsigned* postLen ) {

	char tmp; 

	while ( !(stackEmpty(s)) ) {
		stackTop(s, &tmp);			// obtains elements from the top of stack
		if ( tmp != '(' ) {
			postExpr[(*postLen)++] = tmp;	// adding to the output array
			stackPop(s);
			stackTop(s, &tmp);	
		}
		else {
			stackPop(s);			// obtains left rounded bracket from the top of stack
			return;
		}
	}
}

/*
** Pomocná funkce doOperation.
** Zpracuje operátor, který je předán parametrem c po načtení znaku ze
** vstupního pole znaků.
**
** Dle priority předaného operátoru a případně priority operátoru na
** vrcholu zásobníku rozhodneme o dalším postupu. Délka převedeného 
** výrazu a taktéž ukazatel na první volné místo, do kterého se má zapisovat, 
** představuje parametr postLen, výstupním polem znaků je opět postExpr.
*/
void doOperation ( tStack* s, char c, char* postExpr, unsigned* postLen ) {

		char tmp;
	
		if ( stackEmpty(s) ) {		// case if stack is empty 
			stackPush(s, c);	
			return;
		}
		else
			 stackTop(s, &tmp);	// obtains elements from the top of stack

		if ( tmp == '(' ) {		// case if on the top of stack is left rounded bracket
			stackPush(s, c);
			return;
		}

		switch(c) {
			case '*':		// case for operator "*" (multiplying)
				if ( tmp == '+' || tmp == '-' ) {	// operators with lower priority on the top of stack
					stackPush(s, c);
					return;
				}
				else if ( tmp == '/' || tmp == '*') {	// operators with higher or equal priority on the top of stack
					stackPop(s);
					postExpr[(*postLen)++] = tmp;
					doOperation(s, c, postExpr, postLen);	// recursive calling 
				}
				break;
			case '/':		// case for operator "/" (division)
				if ( tmp == '+' || tmp == '-' ) {	// operators with lower priority on the top of stack
					stackPush(s, c);
					return;
				}	
				else if ( tmp == '*' || tmp == '/' ) {	// operators with higher or equal priority on the top of stack
					stackPop(s);
					postExpr[(*postLen)++] = tmp;
					doOperation(s, c, postExpr, postLen);	// recursive calling	
				}
				break;
			case '+':		// case for operator "+" (addition)
				if ( tmp == '/' || tmp == '*' || tmp == '-' || tmp == '+' ) {	// operators with higher or equal priority on the top of stack
					stackPop(s);
					postExpr[(*postLen)++] = tmp;
					doOperation(s, c, postExpr, postLen);	// recursive calling
				}
				break;
			case '-':		// case for operator "-" (subtraction)
				if ( tmp == '/' || tmp == '*' || tmp == '+' || tmp == '-' ) {	// operators with higher or equal priority on the top of stack
					stackPop(s);
					postExpr[(*postLen)++] = tmp;
					doOperation(s, c, postExpr, postLen);	// recursive calling
				}
				break;
		}
}

/* 
** Konverzní funkce infix2postfix.
** Čte infixový výraz ze vstupního řetězce infExpr a generuje
** odpovídající postfixový výraz do výstupního řetězce (postup převodu
** hledejte v přednáškách nebo ve studijní opoře). Paměť pro výstupní řetězec
** (o velikosti MAX_LEN) je třeba alokovat. Volající funkce, tedy
** příjemce konvertovaného řetězce, zajistí korektní uvolnění zde alokované
** paměti.
**
** Tvar výrazu:
** 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
**    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
**    násobení má stejnou prioritu jako dělení. Priorita násobení je
**    větší než priorita sčítání. Všechny operátory jsou binární
**    (neuvažujte unární mínus).
**
** 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
**    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
**
** 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
**    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
**    chybné zadání výrazu).
**
** 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen 
**    ukončovacím znakem '='.
**
** 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
**
** Poznámky k implementaci
** -----------------------
** Jako zásobník použijte zásobník znaků tStack implementovaný v příkladu c202. 
** Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
**
** Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
**
** Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
** char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
**
** Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
** nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
** by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
** ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
** řetězce konstantu NULL.
*/
char* infix2postfix (const char* infExpr) {
	
	char *postExpr;
	if ( (postExpr = (char *) malloc(MAX_LEN)) == NULL)		// allocating memory for output expression
		return NULL;

	tStack* s;
	if ( (s = malloc(sizeof(char *) * STACK_SIZE)) == NULL) {	// allocating memory for stack
		free(postExpr);
		return NULL;
	}
	stackInit(s);	// initializating stack
		
	unsigned postLen = 0;	// counter position in output expression
	unsigned infLen = 0;	// counter position in input expression

	while ( infExpr[infLen] != '\0' ) { 				// end of input expression
		if ( (infExpr[infLen] >= 'A' && infExpr[infLen] <= 'Z') ||		// operand from input expression
			 (infExpr[infLen] >= 'a' && infExpr[infLen] <= 'z') ||
			 (infExpr[infLen] >= '0' && infExpr[infLen] <= '9')) 
				postExpr[postLen++] = infExpr[infLen];			// adding to output expression
		else if ( infExpr[infLen] == '(' )					// left rounded bracket
				stackPush(s, infExpr[infLen]);				// insert on stack
		else if ( infExpr[infLen] == '*' || infExpr[infLen] == '/' ||		// operators "+-*/"
				  infExpr[infLen] == '-' || infExpr[infLen] == '+' )
				doOperation(s, infExpr[infLen], postExpr, &postLen); 	// compare operators from input expression and on the top of stack
 		else if ( infExpr[infLen] == ')' ) 
				untilLeftPar (s, postExpr, &postLen);			// emptying of stack 
		else if ( infExpr[infLen] == '=' ) {					// last element of input expression
			while ( !(stackEmpty(s)) ) {
				stackTop(s, &postExpr[postLen++]);			// emptying all elements on stack and adding to output expression			
				stackPop(s);	
			}
			postExpr[postLen++] = '=';	
			postExpr[postLen] = '\0';
		}
		infLen++;		
	}	

	free(s); 
	return postExpr;	
}
/* Konec c204.c */
