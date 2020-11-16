////////////////////////////////////////////////////////////////////////////////
//		Turmit prototypowy - "Mr�wka Langtona" z mo�liwo�ci� wariacji na temat
//                   (przykladowy program SYMSHELL'a) 
//------------------------------------------------------------------------------
// Prosta obs�uga grafiki, ale z odtwarzaniem ekranu i obslug� zdarzen.
// POD Dev-Cpp potrzebne s� dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////
// Turmit ma alement aktywny i �rodowisko, tzw. "g�owic�" albo "czo�o" oraz pamie�
// poniewa� turmit jest tak naprawd� 2 wymiarowym uog�lnieniem maszyny Turinga

#include "symshell.h"
#include <stdio.h> //Wyj�cie na konsole a la j�zyk C - printf(....)
#include <math.h>
#include <fstream>

#define NAZWAMODELU  "Turmit_przyspieszony" //U�ycie define u�atwia montowanie sta�ych ���cuch�w
//Wyj�ciowy rozmiar �wiata i "ekranu" symulacji
const int size=700;

unsigned char World[size][size];//Tablica �wiata - wyzerowana na poczatku bo globalna
         //unsigned char �eby by�o od 0 do 255, bo typ char bywa te� "signed" (zaleznie od kompilatora)
unsigned step_counter=0;//Licznik realnych krok�w modelu

void init_world()
//Funkcja do zapoczatkowania �wiata
{
   //World[0][0]=255;//TODO: ewentualna inicjalizacja losowa
}

struct Turmit //Struktura definiuj�ca stan turmita
{
  int x,y;//po�o�enie
  int stan;//I pami�� robocza - np. kierunek
  Turmit(int ix,int iy,int is):x(ix),y(iy),stan(is){} //KONSTRUKTOR!
};

struct //Struktura anonimowa dla kierunk�w ruchu
{
  int dx,dy;
} Directions[4]={{0,-1},{1,0},{0,1},{-1,0}}; //g�ra,prawo,d�,lewo

Turmit LaAnt(size/2,size/2,0); //Inicjalizacja mr�wki na �rodku z kierunkiem "g�ra"

void single_step()
//Funkcja robi�ca jeden krok symulacji
{
    LaAnt.x=( LaAnt.x+Directions[LaAnt.stan].dx //Przesuni�cie po X
    + size ) % size; //i zabezpieczenie �eby nie wyj�� za tablic�
    LaAnt.y=( LaAnt.y+Directions[LaAnt.stan].dy //Przeusniecie po Y
    + size ) % size; //i zabezpieczenie �eby nie wyj�� za tablic�
    
    if(World[LaAnt.y][LaAnt.x]==0)
    {
      plot(LaAnt.x,LaAnt.y,128);
      World[LaAnt.y][LaAnt.x]=1;//Mo�e by� inna liczba - to tylko marker odwiedzin
      LaAnt.stan=(LaAnt.stan+1)%4;//Mr�wka jest czterokierunkowa
    }
    else
    {
      plot(LaAnt.x,LaAnt.y,255);
      World[LaAnt.y][LaAnt.x]=0;
      LaAnt.stan=(LaAnt.stan+4-1)%4;//Mr�wka jest czterokierunkowa
    }    
    //Licznik krok�w mr�wki
    step_counter++;
}

void stats()
//Funkcja do obliczenia statystyk
{
  //TODO: Np �redniej liczny odwiedze� ju� odwiedzonych oraz liczby pustych
}


//Do wizualizacji obs�ugi zdarze�
const int DELA=0;//Jak d�ugie oczekiwanie w obr�bie p�tli zdarze�
const int VISUAL=1000;//Co ile krok�w symulacji odrysowywa� widok
const char* CZEKAM="Klikaj lub patrz! "; //Monit w p�tli zdarze�
int xmouse=10,ymouse=10;//Pozycja ostatniego "kliku" myszy 

//Kilka deklaracja zapowiadaj�cych inne funkcje obs�uguj�ce model
void replot(); //Funkcja odrysowyj�ca
void read_mouse(); // Obslyga myszy - uzywa� o ile potrzebne
void write_to_file(); // Obsluga zapisu do pliku - u�ywa� o ile potrzebne
void screen_to_file(); //Zapis ekranu do pliku (tylko Windows!)

void replot()
//Rysuje co� na ekranie
{
  for(int x=0;x<size;x++)
    for(int y=0;y<size;y++)  
      {
        unsigned z=World[y][x]*20;//Co tam by�o? Wzmocnione
        z%=256; //�eby nie przekroczy� kolor�w
        //z%=512; //Albo z szaro�ciami
        plot(x,y,z);
      }
  printc(size/2,size,128,255,"%06u  ",step_counter);//Licznik krok�w    
  //Ostatnie polozenie kliku - bia�y krzy�yk   
  //line(xmouse,ymouse-10,xmouse,ymouse+10,255);
  //line(xmouse-10,ymouse,xmouse+10,ymouse,255);
}


int main(int argc,const char* argv[])//Potrzebne s� paramatry wywo�ania programu
{
 fix_size(1);       // Czy udajemy �e ekran ma zawsze taki sam rozmiar?
 mouse_activity(0); // Czy mysz b�dzie obs�ugiwana?   
 buffering_setup(1);// Czy b�dzie pamieta� w bitmapie zawartosc ekranu? PAMI�TANIE PRZY�PIESZA!
 shell_setup(NAZWAMODELU,argc,argv);// Przygotowanie okna z u�yciem parametr�w wywo�ania
 init_plot(size,size,0,1);/* Otwarcie okna SIZExSIZE pixeli + 1 wiersz znakow za pikselami*/

 // Teraz mo�na rysowa� i pisac w oknie 
 init_world();
 replot();
 flush_plot();	// Ekran lub bitmapa po inicjalizacji jest gotowa 

 bool not_finished=true;//Zmienna steruj�ca zako�czeniem programu
 unsigned loop=0;    //Do zliczania nawrot�w p�tli zdarze�
 while(not_finished) //P�TLA OBS�UGI ZDARZE�
    { 
      int pom; //NA ZNAK Z WEJ�CIE OKNA GRAFICZNEGO
      loop++; 
      if(step_counter%(VISUAL/10)==0 && //Dla przy�pieszenia tylko czasami sprawdzamy 
         input_ready()) //Czy jest zdarzenie do obs�ugi?
      {
       pom=get_char(); //Przeczytaj nades�any znak
       switch(pom)
       {
       case 'd': screen_to_file();break; //Zrzut grafiki          
       case 'p': write_to_file();break;//Zapis do pliku tekstowego
       case '\r': replot(); flush_plot();break;//Wymagane odrysowanie
       case '\b': read_mouse();break;//Jest zdarzenie myszy
       case EOF:  //Typowe zako�czenie
       case  27:  //ESC
       case 'q':  //Zako�czenie zdefiniowane przez programiste
       case 'Q': not_finished=false;break;             
       default:
            print(0,screen_height()-char_height('N'),"Nie wiem co znaczy %c [%d] ",pom,pom);
            printf("Nie wiem co znaczy %c [%d] ",pom,pom);
            flush_plot();	// Grafika gotowa   
            break;
        }
      }
      else //Symulacja - jako akcja na wypadek braku zdarze� do obslugi
      {
       single_step(); //Nast�pny krok
       if(step_counter%VISUAL==0) //Odrysuj gdy reszta z dzielenia r�wna 0
       {
             flush_plot();//�eby to co rysowane w trakcie cho� przez chwile b�ysn�o
             replot();//Jest najbardziej kosztowny!
             printc(0,screen_height()-char_height('C'),128,(step_counter%3?255:250),CZEKAM);
             flush_plot();// gotowe  - i wlasciwie co chwila wywolywane 
             delay_ms(DELA);//Wymuszenie oczekiwania - �eby p�tla nie zjada�a ca�ego procesora  
       }
      } 
    }
 	    
 printf("Wykonano %d obrotow petli.\nDo widzenia!\n",loop);
 close_plot();/* Zamykamy okno - po zabawie */
 printf("Do widzenia!\n");
 return 0;
}

void read_mouse()
//Procedura obslugi myszy
{ 
   int xpos,ypos,click;
   if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
   {
      xmouse=xpos;ymouse=ypos;
      //TODO - zaimplementowa� je�li potrzebne                                   
      //...
   }                      
}

void write_to_file()
{
     const char* NazwaPliku=NAZWAMODELU ".out";//U�ywamy sztuczki ze zlepianiem sta�ych 
                                        //�a�cuchowych przez kompilator
     std::ofstream out(NazwaPliku); //Nazwa na razie ustalona z g�ry
     //TODO - funkcja powinna zapisa� wyniki modelu do pliku zamiast wyrysowaywa� na ekranie
     //Format - tabela liczb odpowieniego typu rozdzielanych tabulacjami
     //out<<"L i c z b y:\n"<<a[]<<'\t'<<std::endl;
     
     out.close();
}

void screen_to_file()
//Zapis ekranu do pliku (tylko Windows!)
{
   char bufor[255];//Tymczasowe miejsce na utworzenie nazwy pliku
   sprintf(bufor,"%s%06u",NAZWAMODELU,step_counter);//Nazwa + Numer kroku na 6 polach
   dump_screen(bufor);
}

