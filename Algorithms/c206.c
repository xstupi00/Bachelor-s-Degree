	
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, říjen 2017
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu, 
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem, 
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu, 
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
	L->Act = L->First = L->Last = NULL;	// setting all pointers in the list
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	/* while ( L->First ) { 	// dispose list from the first element
		tDLElemPtr tmp = L->First;
		L->First = L->First->rptr;
		free(tmp);	
	}
	L->Last = NULL;
	L->Act = NULL; */

	while ( L->Last ) {		// dispose list from the end of list (last element)
		tDLElemPtr tmp = L->Last;	// obtains last element for removal 
		L->Last = L->Last->lptr;	// shifting to left
		free(tmp);			// removing element
	}
	L->First = NULL;			// setting empty list
	L->Act = NULL;
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	tDLElemPtr new_elem = NULL;

  	if ( (new_elem = malloc(sizeof(struct tDLElem))) == NULL )	// allocation for new element
    		DLError();	// call error status
	else {
		new_elem->data = val;		// set new element data
		new_elem->lptr = NULL;		// set new begin of the list
		new_elem->rptr = L->First;	// set new element right pointer
		if ( !(L->First) )		// case if list is epmty
			L->Last = new_elem;	
		else 
			L->First->lptr = new_elem;	// add-ons in list
		L->First = new_elem;		// set pointer to first element in list
	}
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	tDLElemPtr new_elem = NULL;

  	if ( (new_elem = malloc(sizeof(struct tDLElem))) == NULL )	// allocation for new element
    		DLError();	// call error status
	else {
		new_elem->data = val;		// set new element data
		new_elem->rptr = NULL;		// set new end of the list
		new_elem->lptr = L->Last;	// set new element left pointer
		if ( !(L->Last) )		// case if list is epmty
			L->First = new_elem;
		else
			L->Last->rptr = new_elem;	// add-ons in list
		L->Last = new_elem;		// set pointer to last element in list
	}	
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->First;	// first element is active
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/	
	L->Act = L->Last;	// last element is active
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if ( !(L->First) )	// empty list
		DLError();	// call error status
	else
		*val = L->First->data;	// returns data of first element
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if ( !(L->Last) )	// empty list
		DLError();	// call error status
	else
		*val = L->Last->data;	// returns data of last element
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	if ( L->First ) {	// non-empty list
		tDLElemPtr tmp = L->First; 	// obtains first element
		if ( L->Act == L->First )	// first element is active
			L->Act = NULL;		// loss of activity
		if ( L->First == L->Last )		// one element in list
			L->First = L->Last = NULL;	// list becomes non-active
		else {
			L->First->rptr->lptr = NULL;	// set left pointer of new first element
			L->First = L->First->rptr;	// set new first element
		}
		free(tmp);	
	}
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
	if ( L->Last ) {	// non-empty list
		tDLElemPtr tmp = L->Last; 	// obtains last element
		if ( L->Act == L->Last )	// last element is active
			L->Act = NULL;		// loss of activity
		if ( L->First == L->Last )		// one element in list
			L->First = L->Last = NULL;	// list becomes non-active
		else {
			L->Last->lptr->rptr = NULL;	// set right pointer of new last element
			L->Last = L->Last->lptr;	// set new last element
		}
		free(tmp);	
	}
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	if ( (L->Act) && L->Act != L->Last ) {		 // last element cannot be active and list must be active
		tDLElemPtr tmp = L->Act->rptr;		 // obtains follower of the active element
		if ( L->Act->rptr == L->Last ) {	 // follower is last element
			L->Act->rptr = NULL;		 // set right pointer of new last element
			L->Last = L->Act;		 // set new last element
		}
		else {					  	// follower is not last element
			L->Act->rptr = L->Act->rptr->rptr;	// set right pointer to follower of *** element
			L->Act->rptr->lptr = L->Act;		// add-ons in list
		}
		free(tmp);
	}
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/	
	if ( (L->Act) && L->Act != L->First ) {		// first element cannot be active and list must be active
		tDLElemPtr tmp = L->Act->lptr;		// obtains predecessor of the active element
		if ( L->Act->lptr == L->First ) {	// predecessor is first element
			L->Act->lptr = NULL;		// set left pointer of new first element
			L->First = L->Act;		// set new first element
		}
		else {						// predecessor is not first element
			L->Act->lptr = L->Act->lptr->lptr;	// set left pointer to predecessor of *** element
			L->Act->lptr->rptr = L->Act;		// add-ons in list
		}
		free(tmp);
	}			
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if ( L->Act ) {			 // list must be active
		tDLElemPtr new_elem = NULL;
	  	if ( (new_elem = malloc(sizeof(struct tDLElem))) == NULL )	 // allocation for new element
    			DLError();				// call error status
		else {
			new_elem->data = val;			// set new element data
			new_elem->lptr = L->Act;		// set left pointer of new element to active element
			if ( L->Act == L->Last ) {		// case if last element is active
				new_elem->rptr = NULL;		// set the new end of list
				L->Last = new_elem;		// set new last element
				L->Act->rptr = new_elem;	// add-ons in list
			}
			else {					// active element is another as last
				new_elem->rptr = L->Act->rptr;	// set right pointer of new element to follower active element
				L->Act->rptr = new_elem;	// add-ons in list
				new_elem->rptr->lptr = new_elem; 
			}	
		}
	}	
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if ( L->Act ) {			// list must be active
		tDLElemPtr new_elem = NULL;
	  	if ( (new_elem = malloc(sizeof(struct tDLElem))) == NULL )	// allocation for new element
    			DLError();				// call error status
		else {
			new_elem->data = val;			// set new elemen data
			new_elem->rptr = L->Act;		// set left pointer of new element to active element
			if ( L->Act == L->First ) {		// case if first element is active
				new_elem->lptr = NULL;		// set the new begin of list
				L->Act->lptr = new_elem;	// add-ons in list
				L->First = new_elem;		// set new first element
			}
			else {					// active element is another as first
				new_elem->lptr = L->Act->lptr;	// set left pointer of new element to predecessor active element
				L->Act->lptr = new_elem;	// add-ons in list
				new_elem->lptr->rptr = new_elem;
			}			
		}
	}				
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	if ( !(L->Act) )	// non-active list
		DLError();	// call error status
	else
		*val = L->Act->data;		// returns data of active element
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	if ( L->Act )			// active list
		L->Act->data = val;	// rewrite data of active element
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if ( L->Act ) {		// active list
		if ( L->Act == L->Last )	// last element is active
			L->Act = NULL;		// list becomes non-active
		else				// active element is another as last
			L->Act = L->Act->rptr;	// active element becomes follower actual active element
	}
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	if ( L->Act ) {		// active list
		if ( L->Act == L->First )	// first element is active
			L->Act = NULL;		// list becomes non-active
		else				// active element is another as first
			L->Act = L->Act->lptr;	// active element becomes predecessor actual active element
	}
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	return ( L->Act != NULL ? 42 : 0 );
}

/* Konec c206.c*/
