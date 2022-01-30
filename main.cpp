#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "rwstring.hpp"
using namespace std;


const unsigned int LARGO_CL = 12;
const unsigned int LARGO_PROD = 10;


    const unsigned int LEER = 1;
    const unsigned int MOSTRAR_DATOS=2;
    const unsigned int DESPACHAR_PEDIDOS=3;
    const unsigned int SALIR=4;



template <typename T> struct Nodo {
	T dato;
	Nodo<T>* sig;
};


struct Desp{
    string cliente = "";
    int bultos = 0;
    float valor = 0;
};


struct Ped{
    int id;
    string cliente = "";
    string producto = "";
    float valor = 0;
};


fstream& operator >> (fstream& fs, Ped& pedido){
    fs.read(reinterpret_cast<char*>(&pedido.id), sizeof(Ped::id));
    pedido.cliente = readstring(fs, LARGO_CL);
    pedido.producto = readstring(fs, LARGO_PROD);
    fs.read(reinterpret_cast<char*>(&pedido.valor), sizeof(Ped::valor));
    return fs;
}


ostream& operator << (ostream& os, const Ped& pedido){
    os << setw(3) << pedido.id << "\t" << left << setw(LARGO_CL) << pedido.cliente << "\t"
     << setw(LARGO_PROD) << pedido.producto << "\t" << setw(8) << right << setprecision(2) << pedido.valor;
    return os;
}


ostream& operator << (ostream& os, const Desp& despacho){
    os << left << setw(LARGO_CL) << despacho.cliente << "\t " << right << setw(2) << despacho.bultos << "\t" << setw(8) << setprecision(2) << despacho.valor;
    return os;
}


template <typename T> Nodo<T>* buscar(T clave, Nodo<T>* lista, int (*criterio)(T, T))
{
	while (lista != nullptr && criterio(clave, lista->dato) > 0)
		lista = lista->sig;

	return lista != nullptr && criterio(clave, lista->dato) == 0 ? lista : nullptr;
}


template <typename T> void insertar(T valor, Nodo<T>*& lista, int (*criterio)(T, T))
{
	Nodo<T>* nuevo = new Nodo<T>;
	nuevo->dato = valor;

	Nodo<T>** pp = &lista;
	while (*pp != nullptr && criterio(nuevo->dato, (*pp)->dato) >= 0)
		pp = &((*pp)->sig);
	nuevo->sig = *pp;
	*pp = nuevo;
}


template <typename T> Nodo<T>* extraer(T clave, Nodo<T>*& lista, int (*criterio)(T, T))
{
	Nodo<T> *aux;
	Nodo<T> **pp = &lista;

	while (*pp != nullptr && criterio(clave, (*pp)->dato) > 0)
		pp = &((*pp)->sig);

	if (*pp != nullptr && criterio(clave, (*pp)->dato) == 0) {
		aux = *pp;
		*pp = (*pp)->sig;
		return aux;
	} else {
		return nullptr;
	}
}


template <typename T> bool borrar(T clave, Nodo<T>*& lista, int (*criterio)(T, T))
{
	Nodo<T>* extraido = extraer(clave, lista, criterio);
	if (extraido) {
		delete extraido;
		return true;
	} else {
		return false;
	}
}


template <typename T> T pop(Nodo<T>* &pila)
{
	if (pila == nullptr) {
		std::cout << "Error: pop en pila vacia" << std::endl;
		exit(EXIT_FAILURE);

	}
	T valor = pila->dato;
	Nodo<T>* aux_elim = pila;
	pila = pila->sig;
	delete aux_elim;
	return valor;
}


template <typename T> void agregar(Nodo<T>*& cola, T valor)
{
	Nodo<T>* nuevo = new Nodo<T>;
	nuevo->dato = valor;
	nuevo->sig = nullptr;
	if (cola == nullptr) {
		cola = nuevo;
	} else {
		Nodo<T>* aux = cola;
		while (aux->sig != nullptr)
			aux = aux->sig;
		aux->sig = nuevo;
	}
}


int cliente_Prod_Asc(Ped a, Ped b){
    if(a.cliente == b.cliente){
        if(a.producto == b.producto)
            return 0;
        return a.producto > b.producto ? 1 : -1;
    }
    return a.cliente > b.cliente ? 1 : -1;
}



void Despachar(Nodo<Ped>* listaPedidos, Nodo<Desp>*& listaDespachos){
    Desp despacho;
    Ped pedido;
    Nodo<Ped>* pedidoEnLista = nullptr;
    cout << "DESPACHAR_PEDIDOS" << endl;
    cout << endl;
    cout << "Ingrese el cliente al que desea despachar: ";
    cin >> pedido.cliente;
    cout << "Ingrese el producto que desea despachar: ";
    cin.ignore();
    cin >> pedido.producto;

    pedidoEnLista = buscar(pedido, listaPedidos, cliente_Prod_Asc);
    if(pedidoEnLista == nullptr)
        cout << "No hay pedidos del producto " << pedido.producto << " realizados por el cliente " << pedido.cliente << endl;
    else{
        cout << "Se despacharan los siguientes pedidos:" << endl;
        despacho.cliente = pedidoEnLista -> dato.cliente;
        while(pedidoEnLista != nullptr){
            cout << pedidoEnLista -> dato << endl;
            despacho.bultos++;
            despacho.valor += pedidoEnLista -> dato.valor;
            borrar(pedido, listaPedidos, cliente_Prod_Asc);
            pedidoEnLista = buscar(pedido, listaPedidos, cliente_Prod_Asc);
        }
        agregar(listaDespachos, despacho);
    }
    cout << endl;
}


void LeerDatos(Nodo<Ped>*& listaPedidos){
    cout << "leer datos" << endl;
    cout << endl;
    Ped pedido;
    fstream archivoDatos;
    archivoDatos.open("Datos.bin", ios::binary | ios::in);
    if(not archivoDatos){
        cout << "Error al abrir el archivo de datos" << endl;
        cin.get();
        return;
    }
    cout << fixed;
    while(archivoDatos >> pedido){
        cout << pedido << endl;
        insertar(pedido, listaPedidos, cliente_Prod_Asc);
    }
    cout << endl;
}


int Menu(){
    char opcion;
    cout << "ingrese una opcion" << endl;
    cout << endl;
    cout  << "1: Leer Datos" << endl;
    cout  << "2: Mostrar" << endl;
    cout  << "3: Despachar" << endl;
    cout  << "4: Salir" << endl;
    cin >> opcion;
    while(opcion - '0' < 1 or opcion - '0' > 4){

        cout << "La opcion ingresada no es valida, ingrese una opcion del 1-4" << endl;
        cout  << "1: Leer Datos" << endl;
        cout  << "2: Mostrar" << endl;
        cout  << "3: Despachar" << endl;
        cout  << "4: Salir" << endl;
        cin.ignore();
        cin >> opcion;
    }

    return opcion - '0';
}


void Mostrar(Nodo<Ped>* listaPedidos, Nodo<Desp>* listaDespachos){
    cout << "MOSTRAR_DATOS" << endl;
    cout << endl;
    cout << "Pedidos" << endl;
    cout << endl;
    if(listaPedidos == nullptr)
        cout << "No hay pedidos cargados en la lista" << endl;
    else{
        cout << " ID\tCliente \tProducto\t  Valor" << endl;
        cout << setw(48) << setfill('\xCD') << "" << endl;
        cout << setfill(' ');
        cout << right;
        while(listaPedidos != nullptr){
            cout << listaPedidos -> dato << endl;
            listaPedidos = listaPedidos -> sig;
        }
    }
    cout << endl;
    cout << "Despachos" << endl;
    cout << endl;
    if(listaDespachos == nullptr)
        cout << "No se realizo el despacho de ningun pedido" << endl;
    else{
        cout << "Cliente \tBultos\t   Valor" << endl;
        cout << setw(33) << setfill('\xCD') << "" << endl;
        cout << setfill(' ');
        while(listaDespachos != nullptr){
            cout << listaDespachos -> dato << endl;
            listaDespachos = listaDespachos -> sig;
        }
    }
    cout << endl;
}


template<typename T> void VaciarLista(Nodo<T>*& lista){
    while(lista != nullptr)
        pop(lista);
}

int main(){
    int opcion = Menu();
    Nodo<Ped>* listaPedidos = nullptr;
    Nodo<Desp>* listaDespachos = nullptr;
    while(opcion != SALIR){
        switch(opcion){
            case LEER:
                VaciarLista(listaPedidos);
                VaciarLista(listaDespachos);
                LeerDatos(listaPedidos);
                break;

            case MOSTRAR_DATOS:
                Mostrar(listaPedidos, listaDespachos);
                break;

            case DESPACHAR_PEDIDOS:
                Despachar(listaPedidos, listaDespachos);
        }
        opcion = Menu();
    }
    VaciarLista(listaPedidos);
    VaciarLista(listaDespachos);
    return EXIT_SUCCESS;
}
