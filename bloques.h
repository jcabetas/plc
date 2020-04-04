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
  public:
    uint8_t &operator [](uint16_t);
    static uint16_t numEstados;
    static void init(void);
    static uint8_t addEstado(char *nombre, uint8_t esOut);
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
    virtual int8_t calcula(void) = 0;
    virtual void   print(void) = 0;
    virtual int8_t addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds) = 0;
    virtual const char *diTipo(void) = 0;
    virtual const char *diNombre(void) = 0;
};

class add: public bloque {
    protected:
        uint16_t numOut;
        uint16_t numInputs[4];
    public:
        add(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};


class timer: public bloque {
    protected:
        int16_t numOut;
        int16_t numInput;
        uint16_t cuentaDs;
        uint16_t tiempoDs;
        uint8_t tipoTimer;
    public:
        timer(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};

class OR: public bloque {
    protected:
        int16_t numOut;
        int16_t numInputs[4];
    public:
        OR(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};


class inputTest: public bloque {
    protected:
        uint16_t numOut;
        uint16_t cuentaDs;
        uint8_t horaIni,minIni,segIni,dsIni;
        uint8_t horaFin,minFin,segFin,dsFin;
    public:
        inputTest(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};


class programador;

class zona: public bloque {
    protected:
        programador *program;
        uint16_t numOut;
        uint8_t minutosA;
        uint8_t minutosB;
    public:
        zona(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};


class start: public bloque {
    protected:
        programador *program;
        uint16_t numNombre;
        uint8_t arrancaA;
        uint8_t DOW;
        uint8_t hora;
        uint8_t min;
    public:
        start(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};


class programador: public bloque {
    protected:
        zona *zonas[MAXZONAS];
        start *starts[MAXSTARTS];
        uint8_t estado; // 0 o zonaActiva
        uint16_t segundosQueFaltan;
        uint16_t numNombre;
    public:
        programador(uint8_t numPar, char *pars[]);  // lee desde string
        static uint8_t numProgramadores;
        uint8_t numZonas;
        uint8_t numStarts;
        void asignaZona(zona *zon);
        void asignaStart(start *strt);
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};


#endif /* PLC_BLOQUES_H_ */

