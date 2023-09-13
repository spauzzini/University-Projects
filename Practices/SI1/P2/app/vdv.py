#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from app import database
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
   catalogue = database.db_catalogo()
   genres = database.db_getGenres()
   return render_template('index.html', title = "Lobby", movies=catalogue, genres=genres)

#LogOut:
@app.route('/logout', methods=['GET', 'POST'])
def logout():
    session.pop('usuario', None)
   #session.pop('carrito')
   # session['carrito']=[]
    return redirect(url_for('index'))

#LogIn:
@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        #Request info
        username = request.form['username']
        password = request.form['password']

        if (database.db_login(username, password) == False):
            #No se diferencia el error: usuario sin registrar / contraseña erronea
            return render_template('logInSection.html', message="Ese usuario no existe o la contraseña es incorrecta.")
        # iniciamos sesion:
        session['usuario'] = username
        session.modified = True
        # hace falta gestionar algo del carrito aqui? creo q no pero en git lo hacen
        resp = make_response(redirect(url_for('index')))
        resp.set_cookie('lastUser', username)
        return resp
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
        saldo = random.randint(0,50)
        if (database.db_register(username, password, emailAdd, cCard, address, saldo) == False):
            return render_template('register.html', message="El usuario dado ya existe.")
        session['usuario'] = username
        session.modified = True
        # hace falta gestionar algo del carrito aqui? creo q no pero en git lo hacen
        resp = make_response(redirect(url_for('index')))
        resp.set_cookie('lastUser', username)
        return resp

    return render_template('register.html', title = "Not registered?")

@app.route('/detallePeli/<id>')
def detalle_peli(id):
    peli = database.db_getMovieDetail(id)
    return render_template('detalle_peli.html', peli = peli)

@ app.route('/search_byName', methods=['GET', 'POST'])
def search_byName():
    if request.method == 'POST':
        name = request.form['nombre']
        pelis = database.db_searchByName(name)
        genres = database.db_getGenres()
        return render_template('index.html', movies=pelis, genres=genres)
    else:
        return render_template('index.html')

@ app.route('/getTopMovies', methods=['GET', 'POST'])
def getTopMovies():
    pelis = database.db_getTopSales(2016,2022,7)
    genres = database.db_getGenres()
    return render_template('index.html', movies=pelis, genres=genres)

@ app.route('/search_byGenre', methods=['GET', 'POST'])
def search_byGenre():
    if request.method == 'POST':
        genre = request.form['genero']
        pelis = database.db_searchByGenre(genre)
        genres = database.db_getGenres()
        return render_template('index.html', movies=pelis, genres=genres)
    else:
        return render_template('index.html')

@ app.route('/search_byTopYear', methods=['GET', 'POST'])
def search_ByTopYear():
    if request.method == 'POST':
        year1 = request.form['year1']
        year2 = request.form['year2']
        l = request.form['qtt']
        pelis = database.db_getTopSales(year1,year2,l)
        genres = database.db_getGenres()
        return render_template('index.html', movies=pelis, genres=genres)
    else:
        return render_template('index.html')

@ app.route('/valoraPeli/<id>', methods=['GET', 'POST'])
def valoraPeli(id):
    if request.method == 'POST':
        voto = request.form['valora']
        database.db_valoraPeli(voto, id, session['usuario'])
        return detalle_peli(id)

@ app.route('/borraValoracion/<id>', methods=['GET', 'POST'])
def borraValoracion(id):
    if request.method == 'POST':
        database.db_borraValoracion(id, session['usuario'])
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
    info = database.db_getUserData(session['usuario'])
    return render_template('perfil.html', mail=info[0], ccard=info[1], address=info[2], saldo=info[3])

@ app.route('/getPeli/<id>')
def getPeli(id):
    peli = database.db_getMovie(id)
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
        old = database.db_getUserSaldo(session['usuario'])
        new = old + int(cantidad)
        database.db_updateUserSaldo(new)
    return goPerfil()

@ app.route('/getUserSaldo')
def getUserSaldo():
    saldo = database.db_getUserSaldo(session['usuario'])
    return saldo

@ app.route('/printSaldo', methods=['GET', 'POST'])
def printSaldo():
    if request.method == 'POST':
        saldo = database.db_getUserSaldo(session['usuario'])
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