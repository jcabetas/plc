#ifndef PLC_BLOQUES_H_
#define PLC_BLOQUES_H_

#include <stdint.h>

#define STORESIZE   1000
#define MAXSTATES     50
#define MAXNOMBRES    50
#define NUMPROGRAMADORES 3
#define MAXZONAS      16
#define MAXSTARTS      3



class nombres {
  public:
    static uint16_t incorpora(const char *nombre_p);
    static uint16_t busca(const char *nombre_p);
    static const char *nomConId(uint16_t id);
};

class estados {
  private:
    static uint8_t  estado[MAXSTATES];
    static uint16_t idNom[MAXSTATES];
    static uint8_t  definidoOut[MAXSTATES];
    static uint8_t  idNom2idEstado[MAXSTATES];
  public:
    uint8_t &operator [](uint16_t);
    static uint16_t numEstados;
    static void init(void);
    static uint8_t addEstado(char *nombre, uint8_t esOut, uint8_t *hayError);
    static void ponEstado(uint16_t numEstado, uint8_t valor);
    static uint8_t diEstado(uint16_t numEstado);
    static void printCabecera(void);
    static void print(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    static const char *nombre(uint16_t numEstado);
};


class bloque {
  public:
	bloque();
    virtual int8_t init(void) = 0;
    virtual void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds) = 0;
    virtual void   print(void) = 0;
    virtual void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds) = 0;
    virtual const char *diTipo(void) = 0;
    virtual const char *diNombre(void) = 0;
};



class add: public bloque {
    protected:
        uint16_t numOut;
        uint16_t numInputs[4];
    public:
        add(uint8_t numPar, char *pars[], uint8_t *hayError);
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        void print(void);
        void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};

class NOT: public bloque {
    protected:
        uint16_t numOut;
        uint16_t numInput;
    public:
        NOT(uint8_t numPar, char *pars[], uint8_t *hayError);
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        void print(void);
        void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};


class timer: public bloque {
    protected:
        int16_t numOut;
        int16_t numInput;
        uint16_t cuenta;
        uint16_t tiempo;
        uint8_t tipoCuenta;
        uint8_t horaIni, minIni, secIni, dsIni;
    public:
        timer(uint8_t numPar, char *pars[], uint8_t *hayError);
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        void print(void);
        void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};

class timerNoRedisp: public bloque {
    protected:
        int16_t numOut;
        int16_t numInput;
        uint16_t cuenta;
        uint16_t tiempo;
        uint8_t tipoCuenta;
        uint8_t horaIni, minIni, secIni, dsIni;
    public:
        timerNoRedisp(uint8_t numPar, char *pars[], uint8_t *hayError);
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        void print(void);
        void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};

class delayon: public bloque {
    protected:
        int16_t numOut;
        int16_t numInput;
        uint16_t cuenta;
        uint16_t tiempo;
        uint8_t contando;
        uint8_t tipoCuenta;
        uint8_t horaIni, minIni, secIni, dsIni;
    public:
        delayon(uint8_t numPar, char *pars[], uint8_t *hayError);
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        void print(void);
        void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};

class OR: public bloque {
    protected:
        int16_t numOut;
        int16_t numInputs[4];
    public:
        OR(uint8_t numPar, char *pars[], uint8_t *hayError);
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        void print(void);
        void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};

class flipflop: public bloque {
    protected:
        int16_t numOut;
        int16_t numInputSet;
        int16_t numInputReset;
    public:
        flipflop(uint8_t numPar, char *pars[], uint8_t *hayError);
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        void print(void);
        void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};

class inputTest: public bloque {
    protected:
        uint16_t numOut;
        uint16_t cuentaDs;
        uint8_t horaIni,minIni,segIni,dsIni;
        uint8_t horaFin,minFin,segFin,dsFin;
    public:
        inputTest(uint8_t numPar, char *pars[], uint8_t *hayError);
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        void print(void);
        void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};


class programador;

class zona: public bloque {
    protected:
        programador *program;
        uint16_t numOut;
        uint8_t minutosA;
        uint8_t minutosB;
    public:
        zona(uint8_t numPar, char *pars[], uint8_t *hayError);
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        uint8_t diTiempo(uint8_t esB);
        void ponSalida(uint8_t estado);
        void print(void);
        void addTime(uint16_t msInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};


class start: public bloque {
    protected:
        programador *program;
        uint16_t numNombre;
        uint8_t esB;
        uint8_t DOW;
        uint8_t hora;
        uint8_t min;
    public:
        start(uint8_t numPar, char *pars[], uint8_t *hayError);
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        void   print(void);
        void addTime(uint16_t msInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};


class programador: public bloque {
    protected:
        zona *zonas[MAXZONAS];
        start *starts[MAXSTARTS];
        uint8_t estado; // 0 o zonaActiva
        uint8_t cicloArrancado;
        uint16_t numOutBomba;
        uint16_t numSuspendeConteo;
        uint16_t dsQueFaltan;
        uint16_t numNombre;
    public:
        programador(uint8_t numPar, char *pars[], uint8_t *hayError);
        static uint8_t numProgramadores;
        uint8_t numZonas;
        uint8_t numStarts;
        void asignaZona(zona *zon);
        void asignaStart(start *strt);
        const char *diTipo(void);
        const char *diNombre(void);
        void arranca(uint8_t esB);
        int8_t init(void);
        void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        void   print(void);
        void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};


#endif /* PLC_BLOQUES_H_ */

