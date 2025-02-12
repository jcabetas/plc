import mysql.connector
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import numpy as np
from datetime import datetime

def calculaEnergias(ysolar, yconsumo):
    autoconsumo = 0.0
    importacion = 0.0
    exportacion = 0.0
    for psol,pload in np.nditer([ysolar,yconsumo]):
        if psol>pload:
            autoconsumo += pload
            exportacion += psol - pload
        else:
            autoconsumo += psol
            importacion += pload - psol
    autoconsumo /= 12.0
    importacion /= 12.0
    exportacion /= 12.0
    return autoconsumo, importacion, exportacion

# consulta devuelve idHora,fechaHora,kWhInv,kWhExp,kWhImp,kWhCasa,kWhKona,kWhTesla
#
def rellena(resultados):
    xdata = []
    y1data = []
    y2data = []

    for x in resultados:
        x5 = x[5]       # kWhCasa
        if x5==None:
            x5 = 0.0
        if x5>99 or x5<-99:
            continue
        x2 = x[2]       # kWhInv
        if x2==None:
            x2 = 0.0
        x6 = x[6]       # kWhKona
        if x6==None:
            x6 = 0.0
        x7 = x[7]
        if x7==None:    # kWhTesla
            x7 = 0.0
        xdata.append(x[1])       #fechaHora
        y1data.append(x2)        #inversor
        y2data.append(x5+x6+x7)  #consumo

    xdata = xdata[::-1]
    y1data = y1data[::-1]
    y2data = y2data[::-1]
    return xdata, y1data, y2data

def dibuja(xdata,ydata,y2data,titulo,nomFichero):
    autoconsumo, importacion, exportacion = calculaEnergias(ydata, y2data)
    if len(xdata)>3:
        xplot = np.array(xdata)
        yplot = np.array(ydata)
        y2plot = np.array(y2data)
        plt.figure(figsize=(12,8))
        plt.fill_between(xplot,yplot,step="pre",alpha=0.4,color='yellow')
        plt.fill_between(xplot,y2plot,step="pre",alpha=0.4,color='skyblue')
        plt.plot(xplot,yplot,color='orange',linewidth=1.2)
        plt.plot(xplot,y2plot,color='blue',linewidth=0.8)
        plt.grid()
        plt.title(titulo)
        ax = plt.gca()
        ax.set_xlim(xplot[0],xplot[-1])
        ax.xaxis.set_major_formatter(mdates.DateFormatter("%d/%m"))
        ax.xaxis.set_major_locator(mdates.DayLocator(interval=1))
        miny,maxy = ax.get_ylim()
        hora_actual = datetime.now()
        txt_hora = hora_actual.strftime('%d/%m\n%H:%M')
        props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
        # place a text box in upper left in axes coords
        ax.text(0.05, 0.95, txt_hora, transform=ax.transAxes, fontsize=14,
            verticalalignment='top', bbox=props)
        # pongo exportacion
        propsyellow = dict(boxstyle='round', facecolor='yellow', alpha = 0.5)
        ax.text(0.20, 0.95, '%.1f kWh'%exportacion, transform=ax.transAxes, fontsize=14,
            verticalalignment='top', bbox=propsyellow)
        # pongo autoconsumo
        propsyellow = dict(boxstyle='round', facecolor='lightgreen', alpha = 0.5)
        ax.text(0.40, 0.95, '%.1f kWh'%autoconsumo, transform=ax.transAxes, fontsize=14,
            verticalalignment='top', bbox=propsyellow)
        # pongo importacion
        propsyellow = dict(boxstyle='round', facecolor='skyblue', alpha = 0.5)
        ax.text(0.60, 0.95, '%.1f kWh'%importacion, transform=ax.transAxes, fontsize=14,
            verticalalignment='top', bbox=propsyellow)
        plt.savefig(nomFichero,bbox_inches='tight')
        #plt.show()


mydb = mysql.connector.connect(
  host="localhost",
  user="joaquin",
  password="ronco-jato@3vg10",
  database="logger"
)
mycursor = mydb.cursor()
mycursor.execute("SELECT * FROM balance5min order by idHora desc limit 864;")
myresult = mycursor.fetchall()
xdata, y1data, y2data = rellena(myresult)
dibuja(xdata,y1data,y2data,'Generacion vs Consumo','paneles.png')
mycursor.execute("SELECT * FROM balance5min order by idHora desc limit 2016;")
myresult = mycursor.fetchall()
xdata, y1data, y2data = rellena(myresult)
dibuja(xdata,y1data,y2data,'Generacion vs Consumo','panelesSemana.png')
