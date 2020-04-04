#ifndef PLC_BLOQUES_H_
#define PLC_BLOQUES_H_

#include <stdint.h>

#define STORESIZE   1000
#define MAXSTATES     50
#define MAXNOMBRES    50



class nombres {
  public:
    static uint16_t incorpora(const char *nombre_p);
    static uint16_t busca(const char *nombre_p);
    static const char *nomConId(uint16_t id);
};

class estados {
//  private:

  public:
    static uint8_t  estado[MAXSTATES];
    static uint16_t idNom[MAXSTATES];
    static uint8_t  definidoOut[MAXSTATES];

    uint8_t &operator [](uint16_t);
    static uint16_t numEstados;
    static void init(void);
    static uint8_t addEstado(char *nombre, uint8_t esOut);
    static void ponEstado(uint16_t numEstado, uint8_t valor);
    static uint8_t diEstado(uint16_t numEstado);
    static void printCabecera(void);
    static void print(uint16_t ds);
};


class bloque {
  public:
	bloque();
    virtual int8_t init(void) = 0;
    virtual int8_t calcula(void) = 0;
    virtual void   print(void) = 0;
    virtual int8_t addTime(uint16_t ms) = 0;
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
        int8_t addTime(uint16_t ms);
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
        int8_t addTime(uint16_t ms);
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
        int8_t addTime(uint16_t ms);
};


class inputTest: public bloque {
    protected:
        uint16_t numOut;
        uint16_t cuentaDs;
        uint16_t tiempoIni;
        uint16_t tiempoFin;
    public:
        inputTest(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms);
};


class programador;

class zona: public bloque {
    protected:
        programador *program;
        int16_t numOut;
        uint8_t minutosA;
        uint8_t minutosB;
    public:
        zona(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms);
};


class start: public bloque {
    protected:
        programador *program;
        uint16_t numInput;
        uint8_t AoB;
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
        int8_t addTime(uint16_t ms);
};


class programador: public bloque {
    protected:
        zona *zonas[16];
        start *starts[3];
        uint8_t estado; // 0 o zonaActiva
        uint16_t segundosQueFaltan;
        uint16_t numNombre;
    public:
        programador(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms);
};


#endif /* PLC_BLOQUES_H_ */

