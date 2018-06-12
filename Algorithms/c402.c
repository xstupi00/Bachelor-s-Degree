
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS 
**                                     Implementace: Petr Přikryl, prosinec 1994
**                                           Úpravy: Petr Přikryl, listopad 1997
**                                                     Petr Přikryl, květen 1998
**			  	                        Převod do jazyka C: Martin Tuček, srpen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                         Úpravy: Karel Masařík, říjen 2013
**                                         Úpravy: Radek Hranický, říjen 2014
**                                         Úpravy: Radek Hranický, listopad 2015
**                                         Úpravy: Radek Hranický, říjen 2016
**
** S využitím dynamického přidělování paměti, implementujte NEREKURZIVNĚ
** následující operace nad binárním vyhledávacím stromem (předpona BT znamená
** Binary Tree a je u identifikátorů uvedena kvůli možné kolizi s ostatními
** příklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vložení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní průchod typu pre-order
**     BTInorder ....... nerekurzivní průchod typu in-order
**     BTPostorder ..... nerekurzivní průchod typu post-order
**     BTDisposeTree ... zruš všechny uzly stromu
**
** U všech funkcí, které využívají některý z průchodů stromem, implementujte
** pomocnou funkci pro nalezení nejlevějšího uzlu v podstromu.
**
** Přesné definice typů naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na něj je typu tBTNodePtr. Jeden uzel obsahuje položku int Cont,
** která současně slouží jako užitečný obsah i jako vyhledávací klíč 
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Příklad slouží zejména k procvičení nerekurzivních zápisů algoritmů
** nad stromy. Než začnete tento příklad řešit, prostudujte si důkladně
** principy převodu rekurzivních algoritmů na nerekurzivní. Programování
** je především inženýrská disciplína, kde opětné objevování Ameriky nemá
** místo. Pokud se Vám zdá, že by něco šlo zapsat optimálněji, promyslete
** si všechny detaily Vašeho řešení. Povšimněte si typického umístění akcí
** pro různé typy průchodů. Zamyslete se nad modifikací řešených algoritmů
** například pro výpočet počtu uzlů stromu, počtu listů stromu, výšky stromu
** nebo pro vytvoření zrcadlového obrazu stromu (pouze popřehazování ukazatelů
** bez vytváření nových uzlů a rušení starých).
**
** Při průchodech stromem použijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodušení práce máte předem připraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte 
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c402.h"
int solved;


void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat při průchodech stromem pro zpracování
** uzlu určeného ukazatelem Ptr. Tuto funkci neupravujte.
**/
			
	if (Ptr==NULL) 
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else 
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}
	
/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)  
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;  
}	

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{ 
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK) 
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {  
		S->top++;  
		S->a[S->top]=ptr;
	}
}	

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top--]);
	}	
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}	

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;  
}	

void SPushB (tStackB *S,bool val) {
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
	if (S->top==MAXSTACK) 
		printf("Chyba: Došlo k přetečení zásobníku pro boolean!\n");
	else {
		S->top++;  
		S->a[S->top]=val;
	}	
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Došlo k podtečení zásobníku pro boolean!\n");
		return(NULL);	
	}	
	else {  
		return(S->a[S->top--]); 
	}	
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat. 
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze před prvním použitím binárního
** stromu, protože neuvolňuje uzly neprázdného stromu (a ani to dělat nemůže,
** protože před inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zrušení binárního stromu slouží procedura BTDisposeTree.
**	
** Všimněte si, že zde se poprvé v hlavičce objevuje typ ukazatel na ukazatel,	
** proto je třeba při práci s RootPtr použít dereferenční operátor *.
**/

	*RootPtr = NULL;	// init tree to empty
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vloží do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytvářený strom jako binární vyhledávací strom,
** kde uzly s hodnotou menší než má otec leží v levém podstromu a uzly větší
** leží vpravo. Pokud vkládaný uzel již existuje, neprovádí se nic (daná hodnota
** se ve stromu může vyskytnout nejvýše jednou). Pokud se vytváří nový uzel,
** vzniká vždy jako list stromu. Funkci implementujte nerekurzivně.
**/

	bool found = FALSE;				// auxiliary value to search in tree
	tBTNodePtr index = NULL;
	tBTNodePtr tmp_node = *RootPtr;	// obtaining root the tree

	if ( *RootPtr ) {				// tree is not epmty
		do {
			index = tmp_node;		// save root the tree
			if ( tmp_node->Cont < Content )	// case if content is greather 
				tmp_node = tmp_node->RPtr;	// go to the right subtrees
			else if ( tmp_node->Cont > Content )	// case if content is less
				tmp_node = tmp_node->LPtr;			// go to the left subtrees
			else {
				found = TRUE;				// content is equal to content actual nide
				break;						// finish searching
			}
		} while ( tmp_node );
	}

	if ( !found ) {					// if searching was succesfull
		tBTNodePtr new_element;		// auxiliary node for insert to the tree
		if( !(new_element = (tBTNodePtr) malloc(sizeof(struct tBTNode))) )
			return;	// allocating needed memory for inserting
		new_element->Cont = Content;		// sets new content
		new_element->LPtr = NULL;			// sets empty left subtrees, inserting node must be leaf
		new_element->RPtr = NULL;			// sets empty right subtrees, inserting node must be too leaf
		if ( !index )
			*RootPtr = new_element; 		// if tree was empty, sets new root of tree
		else if ( index->Cont < Content )	// if content is less, inserting to right subtrees
			index->RPtr = new_element;
		else								// content is greather
			index->LPtr = new_element;		// inserting to the left subtrees
	}
}

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Preorder navštívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na ně is uložíme do zásobníku.
**/

	while ( ptr ) {
		SPushP(Stack, ptr);	// save actual node
		BTWorkOut(ptr);		// proccessing content of node
		ptr = ptr->LPtr;	// shift to the next node in the left subtrees
	}	
}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Průchod stromem typu preorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Preorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/

	tStackP stack_ptr;		
	SInitP(&stack_ptr);			// init stack

	Leftmost_Preorder(RootPtr, &stack_ptr);		// obtaining left most leaf in the left tree
	while ( !SEmptyP(&stack_ptr) ) {			// tree is not empty
		tBTNodePtr stack_elem = STopPopP(&stack_ptr);	// obtaining node from top of the stack
		Leftmost_Preorder(stack_elem->RPtr, &stack_ptr);	// obtaining left most leaf in the right subtrees actual nodes
	}
}


/*                                  INORDER                                   */ 

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Inorder ukládáme ukazatele na všechny navštívené uzly do
** zásobníku. 
**/
	
	while ( ptr ) {
		SPushP(Stack, ptr);	// save actual node
		ptr = ptr->LPtr;	// shift to the next node in the left subtrees
	}		
}

void BTInorder (tBTNodePtr RootPtr)	{
/*   ---------
** Průchod stromem typu inorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Inorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/

	tStackP stack_ptr;
	SInitP(&stack_ptr);			// init stack

	Leftmost_Inorder(RootPtr, &stack_ptr);					// obtaining left most leaf in the left tree
	while ( !SEmptyP(&stack_ptr) ) {						// tree is not empty
		tBTNodePtr stack_elem = STopPopP(&stack_ptr);		// obtaining node from top of the stack
		BTWorkOut(stack_elem);								// proccessing content of node
		Leftmost_Inorder(stack_elem->RPtr, &stack_ptr);		// obtaining left most leaf in the right subtrees actual nodes
	}
}

/*                                 POSTORDER                                  */ 

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Postorder ukládáme ukazatele na navštívené uzly do zásobníku
** a současně do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** navštíven poprvé a že se tedy ještě nemá zpracovávat. 
**/

	while ( ptr ) {
		SPushP(StackP, ptr); 	// save actual node
		SPushB(StackB, TRUE);	// first order to actual node
		ptr = ptr->LPtr;		// shift to the next node in the left subtrees
	}	
}

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Průchod stromem typu postorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Postorder, zásobníku ukazatelů a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut(). 
**/

	tStackP stack_ptr;
	SInitP(&stack_ptr);			// init stack for nodes

	tStackB stack_bool;
	SInitB(&stack_bool);		// init stack for boolean values

	Leftmost_Postorder(RootPtr, &stack_ptr, &stack_bool);	// obtaining left most leaf in the left tree
	while ( !SEmptyP(&stack_ptr) ) {
		tBTNodePtr stack_elem = STopPopP(&stack_ptr);	
		SPushP(&stack_ptr, stack_elem);
		bool from_left = STopPopB(&stack_bool);
		if ( !from_left ) {							// second order to actual node
			BTWorkOut(stack_elem);					// proccessing content of node
			stack_elem = STopPopP(&stack_ptr);
		}
		else {
			SPushB(&stack_bool, FALSE);										// first order to node
			Leftmost_Postorder(stack_elem->RPtr, &stack_ptr, &stack_bool); // go to the right subtrees actual nodes
		}
	}
}


void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zruší všechny uzly stromu a korektně uvolní jimi zabranou paměť.
**
** Funkci implementujte nerekurzivně s využitím zásobníku ukazatelů.
**/

	tStackP stack_ptr;
	SInitP(&stack_ptr);			// init stack

	do {
		if ( *RootPtr ) {				// tree is not empty
			if ( (*RootPtr)->RPtr )		// go to the right tree
				SPushP(&stack_ptr, (*RootPtr)->RPtr);	// push right tree on stack
			tBTNodePtr delete_elem = *RootPtr;		
			*RootPtr = (*RootPtr)->LPtr;		// change root of tree to the left successor
			free(delete_elem);					// clean allocated memory
		}
		else if ( !SEmptyP(&stack_ptr) ) 
				*RootPtr = STopPopP(&stack_ptr);
	} while ( *RootPtr || !SEmptyP(&stack_ptr) ); // while deleting all nodes from tree
}

/* konec c402.c */
