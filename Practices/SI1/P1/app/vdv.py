#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from os.path import exists
from tarfile import CompressionError
from app import *
from flask import render_template, request, url_for, redirect, session, make_response
import json
import os
import sys
import hashlib 
import random


#Lobby:
@app.route('/')
@app.route('/index')
def index():
   print (url_for('static', filename='estilo.css'), file=sys.stderr)
   catalogue_data = open(os.path.join(app.root_path,'catalogue/catalogue.json'), encoding="utf-8").read()
   catalogue = json.loads(catalogue_data)
   return render_template('index.html', title = "Lobby", movies=catalogue['peliculas'])

#LogOut:
@app.route('/logout', methods=['GET', 'POST'])
def logout():
    session.pop('usuario', None)
    session.pop('carrito')
    session['carrito']=[]
    return redirect(url_for('index'))

#LogIn:
@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        #Request info
        username = request.form['username']
        password = request.form['password']

        path = os.getcwd() + "/si1users/"
        filename = "userdata.json"

        for dirpath, dirnames, filenames in os.walk(path):
            for dir in dirnames:
               if dir==username:
                #Hemos encontrado la carpeta del usuario q quiere logear asiq guardamos el path
                userpath = os.getcwd() + "/si1users/" + dir
                #Guardamos la ruta al finchero json
                userdata = userpath + "/" + filename 
                #Abrimos el fichero para leer
                i = open(userdata, 'r')
                #Obtenemos la contraseña del fichero del usuario
                json_load = (json.load(i))
                x = json_load['userdata']
                user_key=x["password"]

                #Password encode
                salt = user_key[0]  # A new salt for this user
                salt = str(salt)
                kkey = user_key[1:]
                key = hashlib.sha3_384((salt+password).encode('utf-8')).hexdigest()
                #Comprobamos si las dos contraseñas coinciden
                if key==kkey:
                    #se le inicia sesion al nuevo usuario y le mandamos al lobby
                    session['usuario'] = username
                    session.modified = True
                    resp = make_response(redirect(url_for('index')))
                    resp.set_cookie('lastUser', username)
                    return resp
                else:
                    return render_template('logInSection.html', message="Contraseña incorrecta.")
            else:
                return render_template('logInSection.html', message="Ese usuario no existe.")
    return render_template('logInSection.html', lastUser= request.cookies.get('lastUser'))

#Register:
@app.route('/register', methods=['GET', 'POST'])
def register_page():
    if request.method == 'POST':
        #Request info
        username = request.form['username']
        password = request.form['password']
        rpassword = request.form['rpassword']
        emailAdd = request.form['emailAdd']
        cCard = request.form['cCard']
        address = request.form['address']

        #Algun campo vacio
        if username=="" or password=="" or rpassword=="" or emailAdd=="" or cCard=="" or address=="":
            return render_template('register.html', message="Debe rellenar todos los campos.")

        #Tamaño username erroneo
        if len(username)<5 or " " in username:
            return render_template('register.html', message="El usuario debe tener al menos 5 caracteres y no puede tener ni espacios ni caracteres especiales.")

        #Buscamos la carpeta donde estan nuestros usuarios
        path = os.getcwd() + "/si1users/"
        for dirpath, dirnames, filenames in os.walk(path):
            for dir in dirnames:
                #Usuario ya existe
                if dir==username:
                    return render_template('register.html', message="El usuario introducido ya existe.")
        
        #Tamaño contraseña erroneo
        if len(password)<6:
            return render_template('register.html', message="La contraseña debe tener al menos 6 caracteres.")

        #Contraseña repetida mal
        if password != rpassword:
            return render_template('register.html', message="Las contraseñas no coinciden.")
    
        #Tamaño creditcard erroneo
        if len(cCard)<16:
            return render_template('register.html', message="La tarjeta de crédito debe tener 16 dígitos.")

        #Email invalido
        if "@" not in emailAdd:
            return render_template('register.html', message="Formato email inválido.")
        
        #Tamaño direccion erroneo
        if len(address)>50:
            return render_template('register.html', message="La dirección debe tener 50 caracteres como máximo.")

        #Despues de comprobar cualquier error en los datos introducidos. Ahora sí registramos al usuario.
        #To hasg the password
        salt = random.randint(0, 9)  # A new salt for this user
        salt = str(salt)
        key = hashlib.sha3_384((salt+password).encode('utf-8')).hexdigest()

        #Buscamos path para guardar datos
        path = os.getcwd() + "/si1users/" + username
        os.mkdir(path)

        #Creamos fichero
        userdata = path + "/" + "userdata.json"
        historial = path + "/compras.json"

        #saldo aleatorio para el usuario
        saldo = random.randint(0,50)
        
        #Creamos estructura de datos con la info del usuario
        baseData = {"userdata": { "username": username, "password": salt+key, "email": emailAdd, "creditcard": cCard, "address": address, "saldo": saldo, "totalCompras": 0}}

        hist = {"Historial": []}

        #Abrimos el archivo donde guardaremos la estructura
        h = open(userdata, 'w')
        m = open(historial, 'w')
        #Guardamos los datos de la estructura
        json.dump(baseData, h, indent=4)
        json.dump(hist, m)
        #Cerramos el archivo
        h.close()
        m.close()

        #se le inicia sesion al nuevo usuario y le mandamos al lobby
        session['usuario'] = username
        session.modified = True
        resp = make_response(redirect(url_for('index')))
        resp.set_cookie('lastUser', username)
        return resp

    return render_template('register.html', title = "Not registered?")

@app.route('/detallePeli/<id>')
def detalle_peli(id):
    path = os.getcwd() + "/app/catalogue/catalogue.json"
    json_file = open(path, "r")
    catalogo = json.loads(json_file.read())
    peli = catalogo['peliculas'][int(id)-1]
    return render_template('detalle_peli.html', peli = peli)

@ app.route('/search_byName', methods=['GET', 'POST'])
def search_byName():
    if request.method == 'POST':
        name = request.form['nombre']
        path = os.getcwd() + "/app/catalogue/catalogue.json"
        json_file = open(path, "r")
        catalogo = json.loads(json_file.read())
        pelis = catalogo['peliculas']

        pelis = [item for item in pelis if item['titulo'].lower().find(name.lower()) != -1]
        return render_template('index.html', movies=pelis)
    else:
        return render_template('index.html')

@ app.route('/search_byGenre', methods=['GET', 'POST'])
def search_byGenre():
    if request.method == 'POST':
        genre = request.form['genero']
        path = os.getcwd() + "/app/catalogue/catalogue.json"
        json_file = open(path, "r")
        catalogo = json.loads(json_file.read())
        pelis = catalogo['peliculas']
        pelis = [item for item in pelis if item['categoria'] == genre]
        return render_template('index.html', movies=pelis)
    else:
        return render_template('index.html')


@ app.route('/valoraPeli/<id>', methods=['GET', 'POST'])
def valoraPeli(id):
    if request.method == 'POST':
        voto = request.form['valora']
        path = os.getcwd() + "/app/catalogue/catalogue.json"
        json_file = open(path, "r")
        catalogo = json.loads(json_file.read())
        pelis = catalogo['peliculas']
        for item in pelis:
            if item['id']==int(id):
                media = int(item['media']) * (int(item['nvotos']))/(int(item['nvotos'])+1) + (int(voto)/(int(item['nvotos'])+1))
                with open(path, "r") as jsonFile:
                    data = json.load(jsonFile)
                data['peliculas'][(int(id)-1)]['media']= media
                data['peliculas'][int(id)-1]['nvotos']+=1
                with open(path, "w") as jsonFile:
                    json.dump(data, jsonFile, indent=4)
                return detalle_peli(id)

        return detalle_peli(id)

@ app.route('/cesta')
def goCesta():
    return render_template('cesta.html', carro=session['carrito'])

@ app.route('/goOkCompra')
def goOkCompra():
    return render_template("okcompra.html")

@ app.route('/goNotOkCompra')
def goNotOkCompra():
    return render_template("notokcompra.html")

@ app.route('/historial')
def historial():
    path = os.getcwd() + "/si1users/" + session['usuario'] + "/compras.json"
    json_file = open(path, "r")
    pedidos = json.loads(json_file.read())
    pedidos = pedidos['Historial']
    return render_template("historial.html", pedidos=pedidos)

@ app.route('/goPerfil')
def goPerfil():
    path = os.getcwd() + "/si1users/" + session['usuario'] + "/userdata.json"
    with open(path, "r") as jsonFile:
        data = json.load(jsonFile)
    mail = data['userdata']['email']
    ccard = data['userdata']['creditcard']
    address = data['userdata']['address']
    saldo = data['userdata']['saldo']
    return render_template('perfil.html', mail=mail, ccard=ccard, address=address, saldo=saldo)

@ app.route('/getPeli/<id>')
def getPeli(id):
    path = os.getcwd() + "/app/catalogue/catalogue.json"
    json_file = open(path, "r")
    catalogo = json.loads(json_file.read())
    pelis = catalogo['peliculas']
    for item in pelis:
        if item['id']==id:
            peli=item
    return peli

@ app.route('/addToCarrito/<id>', methods=['GET', 'POST'])
def addToCarrito(id):
    #buscamos en el catalogo la pelicula deseada
    if request.method == 'POST':
        p=getPeli(int(id))
        #Comprobamos si la cesta esta creada, si sí
        if 'carrito' in session:
            #comprobar si la peli deseada esta ya en la cesta
            for x in session['carrito']:
                if x['id'] == id:
                    x['cantidad']+= 1
                    session.modified = True
                    return detalle_peli(id)
            # si no:
            session['carrito'].append({'id': id, 'nombre': p['titulo'],'cantidad': 1, 'precio': p['precio']})
            session.modified = True
            return detalle_peli(id)

        #No hay cesta, la creamos
        else:
            session['carrito']= []
            session['carrito'].append({'id': id,'nombre': p['titulo'], 'cantidad': 1, 'precio': p['precio']})
            session.modified = True
            return detalle_peli(id)
    return detalle_peli(id)


@ app.route('/emptyCarrito', methods=['GET', 'POST'])
def emptyCarrito():
    session.pop('carrito')
    session['carrito']=[]
    return render_template("cesta.html", carro = session['carrito'])

@ app.route('/delFromCarrito/<id>', methods=['GET', 'POST'])
def delFromCarrito(id):
    aux=0
    for item in session['carrito']:
        if item['id'] == id:
            del session['carrito'][aux]
        aux+=1
    return render_template("cesta.html", carro = session['carrito'])

@ app.route('/addOneCarrito/<id>', methods=['GET', 'POST'])
def addOneCarrito(id):
    for item in session['carrito']:
        if item['id'] == id:
            item['cantidad']+=1
    return render_template("cesta.html", carro = session['carrito'])

@ app.route('/delOneCarrito/<id>', methods=['GET', 'POST'])
def delOneCarrito(id):
    for item in session['carrito']:
        if item['id'] == id:
            if item['cantidad']>1:
                item['cantidad']-=1
            else:
                delFromCarrito(id)
    return render_template("cesta.html", carro = session['carrito'])

@ app.route('/addSaldo/', methods=['GET', 'POST'])
def addSaldo():
    if request.method == 'POST':
        cantidad = request.form['cantidad']
        if int(cantidad)<=0:
            return goPerfil()
        path = os.getcwd() + "/si1users/" + session['usuario'] + "/userdata.json"
        with open(path, "r") as jsonFile:
            data = json.load(jsonFile)
        newSaldo = data['userdata']['saldo'] + int(cantidad)
        data['userdata']['saldo'] = newSaldo
        with open(path, "w") as jsonFile:
            json.dump(data, jsonFile, indent=4)
    return goPerfil()

@ app.route('/getUserSaldo')
def getUserSaldo():
    path = os.getcwd() + "/si1users/" + session['usuario'] + "/userdata.json"
    with open(path, "r") as jsonFile:
        x = json.load(jsonFile)
    saldo=x['userdata']['saldo']
    return saldo

@ app.route('/printSaldo', methods=['GET', 'POST'])
def printSaldo():
    if request.method == 'POST':
        path = os.getcwd() + "/si1users/" + session['usuario'] + "/userdata.json"
        with open(path, "r") as jsonFile:
            x = json.load(jsonFile)
        saldo=x['userdata']['saldo']
        return render_template("base.html", saldo=saldo)
           
@app.route('/payCarrito', methods=['GET', 'POST'])
def payCarrito():
    #Si no hay una sesion iniciada, no se procede al pago y se pide que inicie sesion
    if request.method == 'POST':
        if 'usuario' not in session:
            return render_template("logInSection.html")

        #Si hay un usuario con sesión iniciada, procedemos con el pago
        else:
            mySaldo = getUserSaldo()
            #Obtenemos el total del carrito
            totalcarrito = sumaCarrito()

            #Comprobamos que el usuario tenga suficiente saldo para pagar todo lo que hay en el carrito
            if totalcarrito>mySaldo or mySaldo==0:
                #Si no tiene suficiente saldo, se le notifica y no se completa el pago
                return goNotOkCompra()
            else:
                #Actualizamos el nuevo saldo del usuario:
                path = os.getcwd() + "/si1users/" + session['usuario'] + "/userdata.json"
                with open(path, "r") as jsonFile:
                    data = json.load(jsonFile)
                newSaldo = mySaldo-totalcarrito
                totalCompras=data['userdata']['totalCompras']
                data['userdata']['saldo'] = newSaldo
                data['userdata']['totalCompras'] +=1
                with open(path, "w") as jsonFile:
                    json.dump(data, jsonFile, indent=4)

                #Ahora generamos el ticket de compra y lo guardamos en el historial de compra del usuario
                #Generamos estructura con los datos de esta compra:
                newCompra = generarTicket(totalcarrito)
                #Guardamos estructura en el fichero de historial:
                historial = os.getcwd() + "/si1users/" + session['usuario'] + "/compras.json"
                with open(historial, "r") as jsonFile:
                    x = json.load(jsonFile)
                
                hist = "Compra "+str(totalCompras)
                j={hist: newCompra}
                currentList=x['Historial']
                with open(historial, "r+") as jsonFile:
                    currentList.append(j)
                    jsonFile.seek(13)
                    json.dump(currentList, jsonFile, indent=4)

                with open(historial, "a") as jsonFile:
                    jsonFile.write("}")
                    
                #Ahora que ya hemos finalizado la compra, borramos el carrito
                session.pop('carrito')
                session['carrito']=[]
                return goOkCompra()

@app.route('/sumaCarrito')
def sumaCarrito():
    total=0
    for item in session['carrito']:
        total += item['precio'] * item['cantidad']
    return total

@app.route('/generarTicket')
def generarTicket(precio):
    a=""
    for item in session['carrito']:
        a += "{DVD: " + str(item['nombre']) + ", Cantidad: " + str(item['cantidad']) + ", PpP: " + str(item['precio']) + "} "
    s = {"Resumen": a, "Total pagado": precio}
    return s