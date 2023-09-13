'''
    practica1.py
    Muestra el tiempo de llegada de los primeros 50 paquetes a la interfaz especificada
    como argumento y los vuelca a traza nueva con tiempo actual

    *Realizamos una modificación para que en lugar de ser los 50 primeros paquetes, sean
    cualquier número de paquetes sin limite hasta que se pulse control+c

    Autor: Javier Ramos <javier.ramos@uam.es>
    2020 EPS-UAM
'''

from rc1_pcap import *
import sys
import binascii
import signal
import argparse
from argparse import RawTextHelpFormatter
import time
import logging

ETH_FRAME_MAX = 1514
PROMISC = 1
NO_PROMISC = 0
TO_MS = 10
num_paquete = 0
TIME_OFFSET = 30*60

def signal_handler(nsignal,frame):
	if handle:
		pcap_breakloop(handle)
		

def procesa_paquete(us,header,data):
	global num_paquete, pdumper
	logging.info('Nuevo paquete '+ str(num_paquete+1) +' de {} bytes capturado en el timestamp UNIX {}.{}'.format(header.len,header.ts.tv_sec,header.ts.tv_sec))
	num_paquete += 1
	#TODO imprimir los N primeros bytes
	for i in range(args.nbytes):
		if(header.caplen < args.nbytes):
			if(i==header.caplen):
				break
		print(data[i:i+1].hex(), end=' ')
	print("\n")
	#Escribir el tráfico al fichero de captura con el offset temporal
	if args.interface:
		header.ts.tv_sec += 1800
		pcap_dump(pdumper, header, data)


if __name__ == "__main__":
	global pdumper,args,handle
	parser = argparse.ArgumentParser(description='Captura tráfico de una interfaz ( o lee de fichero) y muestra la longitud y timestamp de los N primeros paquetes hasta que se pulse Ctrl+C',
	formatter_class=RawTextHelpFormatter)
	parser.add_argument('--file', dest='tracefile', default=False,help='Fichero pcap a abrir')
	parser.add_argument('--itf', dest='interface', default=False,help='Interfaz a abrir')
	parser.add_argument('--nbytes', dest='nbytes', type=int, default=14,help='Número de bytes a mostrar por paquete')
	parser.add_argument('--debug', dest='debug', default=False, action='store_true',help='Activar Debug messages')
	args = parser.parse_args()

	if args.debug:
		logging.basicConfig(level = logging.DEBUG, format = '[%(asctime)s %(levelname)s]\t%(message)s')
	else:
		logging.basicConfig(level = logging.INFO, format = '[%(asctime)s %(levelname)s]\t%(message)s')

	if args.tracefile is False and args.interface is False:
		logging.error('No se ha especificado interfaz ni fichero')
		parser.print_help()
		sys.exit(-1)

	signal.signal(signal.SIGINT, signal_handler)

	errbuf = bytearray()
	handle = None
	pdumper = args.tracefile
	
	
	#TODO abrir la interfaz especificada para captura o la traza
	if args.interface:
		handle = pcap_open_live(args.interface, ETH_FRAME_MAX, 0, 100, errbuf)
		if handle == None:
			logging.error("Ha habido un error al capturar la interfaz")
		#p será el archivo donde volquemos los paquetes. Por tanto, hay que abrirlo
		p = pcap_open_dead(DLT_EN10MB, 1514) 
		#pdumper (definido previamente) será la traza donde guardaremos los paquetes. Según el enunciado de la práctica
		#nombre captura.nombreitf.FECHA.pcap (donde FECHA será el tiempo actual UNIX en segundos y nombreitf el nombre de la interfaz especificada).
		pdumper = pcap_dump_open(p, "captura." + args.interface + "." + str(int(time.time())) + ".pcap")
		if(pdumper == None):
			logging.error("Se produjo un error al guardar el dumper")

	#TODO abrir un dumper para volcar el tráfico (si se ha especificado interfaz) 
	if args.tracefile:
		handle = pcap_open_offline(args.tracefile, errbuf)
		if handle == None:
			logging.error("Se produjo un error al abrir el archivo")
	
	
	#La función pcap_loop() se utiliza para leer el tráfico
	ret = pcap_loop(handle,-1,procesa_paquete,None)
	if ret == -1:
		logging.error('Error al capturar un paquete')
	elif ret == -2:
		logging.debug('pcap_breakloop() llamado')
	elif ret == 0:
		logging.debug('No mas paquetes o limite superado')
	logging.info('Control C pulsado')
	logging.info('{} paquetes procesados'.format(num_paquete))

	#TODO si se ha creado un dumper cerrarlo
	if args.interface:
		pcap_close(p) #Abierto previamente con pcap_open_dead, luego lo cerramos
		pcap_dump_close(pdumper) #Abierto previamente con pcap_dump_open, luego lo cerramos

	pcap_close(handle) #Abierto previamente con pcap_open_live, luego lo cerramos
	
	

