
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, říjen 2014
**                              Radek Hranický, listopad 2015
**                              Radek Hranický, říjen 2016
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {

	if ( !ptrht ) 	// control pointer is valid
		return;
	else
		for ( unsigned index = 0; index < HTSIZE; index++ ) // crossing through all items of table
			(*ptrht)[index] = NULL;							// sets all pointers to NULL
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {

	if ( *ptrht && key ) {				// control that pointer to the table is valid and that key is too valid
		tHTItem* search_item = NULL;					// creating new items for searching
		unsigned index = hashCode(key);					// obtaining proper index to the table
		search_item = (*ptrht)[index];					// obtaining propes pointers to the item in table
		while ( search_item ) {					// case for empty item in table
			if ( strcmp(search_item->key, key) == 0 )	// compare keys
				return search_item;						// succesfull
			search_item = search_item->ptrnext;			// shift on successor
		}
	}

	return NULL;	// failure searching
}

/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {

	tHTItem* insert_item = htSearch(ptrht, key); 	// control that item with key yet existing

	if ( !insert_item ) {							// item with key dont existing
		unsigned new_index = hashCode(key);			// obtaining proper index to the table
		tHTItem *new_item;							// item for insert to the table
		if ( (new_item = (tHTItem *) malloc(sizeof(struct tHTItem))) == NULL )
			return;									// allocation new item and control success
		if ( (new_item->key = (tKey) malloc((strlen(key)+1))) == NULL ) {
			free(new_item);							// allocation key (string) in new item
			return;									// in case of failute clear allocated memory
		}
		new_item->data = data;						// insert data
		strcpy(new_item->key, key);					// copy key
		new_item->ptrnext = (*ptrht)[new_index];	// set successor new item
		(*ptrht)[new_index] = new_item;				// set new first item in proper index
	}
	else 
		insert_item->data = data;
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {

	tHTItem* search_item = htSearch(ptrht, key);	// search searching item in table according to key
	if ( search_item )								// item with key existing
		return &search_item->data;					// return proper data
	else
		return NULL;								// item with searchig key dont exist

}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {

	if ( *ptrht && key ) {				// control that pointer to the table is valid and that key is too valid
		unsigned search_index = hashCode(key);		 	// obtaining proper index to the table
		tHTItem *search_item = (*ptrht)[search_index]; 	// obtaining propes pointers to the item in table
		tHTItem *prev_item = NULL;					 	// set auxiliary items
		tHTItem *next_item = NULL;
		while ( search_item ) {					// tem with key existing
			next_item = search_item->ptrnext;			// save successor actual item
			if ( strcmp(search_item->key, key) == 0 ) {	// compare keys, in continue if case success
				if ( prev_item )						// control that actual item have predcessor
					prev_item->ptrnext = search_item->ptrnext;	// save, if have
				else 
					(*ptrht)[search_index] = search_item->ptrnext;	// set first item in the proper index, if havent
				if ( search_item->key )					// clear allocated memory
					free(search_item->key);
				free(search_item);
			}
			prev_item = search_item;					// shift in "list" on propes index, sets needed items
			search_item = next_item;
		}
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {

	tHTItem *tmp_item = NULL;								// auxiliary item for delete from table
	for ( unsigned index = 0; index < HTSIZE; index++) {	// crossing through all items of table
		
		while ( (*ptrht)[index] ) {					// crossing through "lists" of item in proper index in table
			tmp_item = (*ptrht)[index];						// obtaining actual item 
			(*ptrht)[index] = (*ptrht)[index]->ptrnext;		// set "begin" list to the successor of actual
			if ( tmp_item->key )			
				free(tmp_item->key);						// clear allocated memory
			free(tmp_item);
		}
		
	}
}
