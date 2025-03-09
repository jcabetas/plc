#Errores
   - El TIM2 no es adecuado para cargador, ya que solo puedo seleccionar para inyected el CH1 (que coincide con la salida PWM)
     => debería usar TIM1. No obstante, he logrado poner TIM3 como esclavo simultáneo con TIM2 

#Uso de Timers
   - TIM2: genera PWM para cargador (CH1), y para arranque del conversor ADC1, tanto regular (CH2) como inyectado (CH3) 

#Cargador refinado
   Cada aparato define su cargador
   - CARGADOR [idCharger.ajustes.picc 0] car.base.picc.0 sensRCar SP 16 Pve kWhTot kWhParc 3 Ive Ive Ive 
   - CARG-AJUSTES ajustes [ajustFijoEscMaestro.ajustes.picc.4 0] \[Ifija 10] Ptot Pmax [Ppunta 10000] \[Pvalle 15000] PermP PermL PermV coches
   - CANBUS 1
   - Los modos son: fijo (sujeto a punta, llano y valle propios), esclavo (solo carga cuando el maestro se lo indica) y maestro (manda ajustes a todos los cargadores existentes) 
   - Cada cargador en principio es autosuficiente. Si tiene medidas de casa se puede poner en modo maestro, si no, sólo en carga fija hasta que reciba esa medida
   - Si un cargador maestro oye a otro declararse como maestro, cambia a modo esclavo
   - La medida de casa se debe publicar explicitamente en CAN con idMed=99. El resto de medidas de cargadores lo envian los propios cargadores
   - ¿Como podemos sacar medidas de coches en Nextion/web y decidir cuales si o no?? Pues a huevo: lo sacamos en la página correspondiente y en la www si esta definido en el cargador
   - El cargador debe definir todas las medidas y estados de los coches que pueda recibir por CAN

#Publicación CAN
   - Cada cargador tiene un SID (1..5) que se usa en CAN para identificar
   - El cargador publica sus características y ajustes cada 10s que sirve ademas como "ping"
   - El cargador también publica sus medidas, cambios de estados y ajustes
 
#Cambios en interfaz
  - La figura de la portada poner solo el coche principal
  - Añadir boton de prioritario para la carga actual (si no, se usará el ID)
  - Añadir una página con datos de cargadores (ID, maestro/Esclavo/Fijo, #fases, I, P, kWh)   
   

#Funcionamiento
  - Sólo se define el cargador propio en el fichero
  - Cuando arranca, publica las características por CAN (tipo 5: iMax, numFasesMax)
  

# Gestor de Pozo

## uControlador

The demo runs on an ST STM32F4 board. See folder "doc"


## Build Procedure

First [Download ChibiStudio](http://www.chibios.org/dokuwiki/doku.php?id=chibios:downloads:start).
Install an STLink V2 programmer. ST Discover boards have one included.
OpenOcd configuration is _-f interface/stlink.cfg -f target/stm32f4x.cfg_


## Nextion interface
**Ajustes de parametros**   -
- Definimos la clase "parametro" que expone un valor uint16_t para ajustes(p.e. cte. de tiempo timer)
- En nextion se crea una variable txt (aunque el valor sea entero)
- Desde nextion se envia orden set p.e. @set,test.tRetardo,18
- Desde plc se localiza la variable en flash, se graba y contesta a nextion actualizando valor p.e. test.tRetardo.txt="18" y test.tRetardo.pco=0 (poner negro)
- En flash, las variables usan el nombre (no la pagina Nextion) como identificador
- Similar con variables de texto (usan str50)

 **Uso de modelos** 
 - Los modelos representan timers, programadores, selectores, etc., y tienen variables internas, ajustes y ordenes que se intercambian con Nextion. Si una variable tiene definida pagina nextion, cuando cambie se debe actualizar en Nextion   
   Cada modelo se identifica normalmente por el nombre de la variable de salida, o nombre especifico si lo hay (p.e. riego)
 - "sino": _SINO test.smsOn 0 [test.modLlamGraph 10]_   
   Variable interna  _selecSwitch_  (usa parametro uint16_t: 0 off, 1 on)
   Variable de salida (0, 1)  
   Opcionalmente  _picBase_ y _picNextion_ (p.e. test.picLlamacion), que pondra en nextion el picNumero (picBase+0,1 para off, on)  
   Orden desde Nextion: @orden,smsOn,toggle (cambiara secuencialmente) o @orden,smsOn,1 (poner en On), el plc actualizar� el estado del pic si esta definido, o la variable txt en nextion si esta definida la pagina  
 - "sinoauto": _SINOAUTO test.modoLlamacion 0 [test.modLlamGraph 10]_   
   Variable interna  _selecSwitch_  (usa parametro uint16_t: 0 off, 1 on y 2: auto),  _estadoInp_ (variable entrada)
   Variable de salida (0, 1)  
   Opcionalmente  _picBase_ y _picNextion_ (p.e. test.picLlamacion), que pondra en nextion el picNumero (picBase+0,1,2,3 para off, on, autoOn y autoOff)  
   Orden desde Nextion: @orden,modoLlamacion,toggle (cambiara secuencialmente) o @orden,modoLlamacion,1 (poner en On), el plc actualizara el estado del pic si esta definido, o la variable txt en nextion si esta definida la pagina  
   Si esta definido el grafico, cuando cambie variable de estado se actualizara en Nextion el estado del pic con modoLlamacion.pic=10+x (2 o 3)   
   - "programador": _PROGRAMADOR riego test.bombaOn test.suspRiego [test.txtRiego]_   
   Orden desde Nextion: @orden,riego,start,a-b o bien @orden,riego,zona,1-n,a-b o @orden,riego,stop   
   Cuando cambie de estado el plc envia a Nextion el estado en modo texto ("regando almendros", "esperando permiso",..)  
   - "zona": _ZONA riego Arizonicas [riego.TArizonicasA 5 0 60] [riego.TArizonicasB 0 0 60]_   
   No necesita ordenes ni ajustes mas alla de los que se pueden poner en parametros (opcionales)
   
         	

## Parametros
* Son variables ajustables que se guardan en flash
* Se definen en fichero plc, con formato paginaNextion.nombre
* El objeto sabe el tipo de variable y como usarlo
* El objeto tiene los nombres y tambien el checksum
* En la flash se guarda: nombrePage, nombreVar, tipoVar, cheksum (de NombreVar)

Proceso:
* El bloque PLC conoce el tipo, nomPage, nomVar, valor inicial, minimo y maximo
* al incializar calcula checksum, busca en flash si hay variables con identica checksum
  tipo y nombreVar  
  -si hay, lee el valor  
  -si no hay, lo crea con el valor por defecto que le pasa el PLC
* Los ajustes a parametros los gestiona Nextion (incluido m�ximo y minimo)

 
## Estados
- Son las variables usadas por los bloques PLC. Si la p�gina est� activa se actualizar� con
  cada cambio; en caso contrario cuando haya huecos. El bloque puede acceder a la p�gina
  activa y decidir (p.e. un medidor de tensi�n s�lo se actualizar�a si hay cambios grandes
  o ha pasado un tiempo m�nimo)

ejemplo:  
_TIMER test.outTimer pozo.bombaOn [ajustes.Timersalida 5 1 48] ho_
    
* TIMER define un parametro tipo U16 (lo decide �l) llamado TimerSalida, en pagina Nextion
    con "ajustes" valorIni igual a 5, min: 1, max:48
* Tambien define un estado 'bombaOn'  en pagina 'pozo'


## Forma de uso con Nextion
  - Los parametros de ajuste se hacen definiendolos en el PLC con []. Se ajustan desde nextion, enviando p.e.  _@set,test.tRetardo,18_  y de vuelta el PLC escribe en Nextion el valor TXT validado (para poderescribir errores)    
  - Adicionalmente se dispone de los modelos  _sino, sinoauto_  para los ajustes l�gicos, enviando desde Nextion ordenes del tipo  _@orden,smsOn,toggle_  . El PLC actualiza el PIC correspondiente  
  - Los estados se definen en PLC; si tienen p�gina Nextion cuando cambie se actualiza (val) en Nextion, y tambi�n se pueden forzar desde Nextion con _@setEstado,riego.suspender,valor (0,1)_  
  - Adicionalmente los bloques pueden actualizar objetos Nextion directamente, que preferentemente se ajustan como PIC  

  
## Lógica de estados
  - El PLC usa variables de estado, que pueden estar o no vinculadas en Nextion
  - Todas las variables de estado tienen su representación nextion
   