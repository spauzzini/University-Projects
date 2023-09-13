# -*- coding: utf-8 -*-

import os
import sys, traceback
from sqlalchemy import create_engine
from sqlalchemy import Table, Column, Integer, String, MetaData, ForeignKey, text
from sqlalchemy.sql import select
from sqlalchemy.sql.expression import null

# configurar el motor de sqlalchemy
db_engine = create_engine("postgresql://alumnodb:alumnodb@localhost/si1", echo=False)
db_meta = MetaData(bind=db_engine)

"""--------- U S U A R I O ----------"""
def db_getUserID(username):
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()
    x = list(db_conn.execute("SELECT customerid FROM Customers WHERE username='"+str(username)+"'"))
    id = x[0][0]
    # Desconectamos la base de datos
    db_conn.close()
    return id

def db_login(username, password):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # buscamos el usuario a partir del username y conseguimos su password
        myUser = list(db_conn.execute("SELECT password FROM Customers WHERE username='"+str(username)+"'"))
        # comprobar si existe el usuario
        if (len(myUser)<=0):
            # No existe
            db_conn.close()
            return False
        # Desconectamos la base de datos
        db_conn.close()
        # comprobar que las contraseñas coinciden
        myPassword = myUser[0][0]
        if (password == myPassword):
            return True
        else:
            return False
    except:
        if (db_conn is not None):
            db_conn.close()
        print("Exception in DB access:")
        print("-"*60)
        traceback.print_exc(file=sys.stderr)
        print("-"*60)
        return False

def db_register(nick, psw, emailAdd, cCard, ads, bal):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # comprobamos que no exista un usuario con ese nombre
        myUser = db_conn.execute("SELECT customerid FROM Customers WHERE username='"+str(nick)+"'")
        # ya existe porque ha devuelto una fila con el id de ese usuario
        if myUser.rowcount > 0:
            db_conn.close()
            return False
        # no existe un usuario con ese nombre, creamos el usuario
        # hace falta el limit 1?
        ids = list(db_conn.execute("SELECT customerid FROM Customers order by customerid desc limit 1"))[0]
        newID = ids[0] + 1
        a = text("INSERT INTO Customers(customerid, address, email, creditcard, username, password, balance) VALUES(:customerid, :address, :email, :creditcard, :username, :password, :balance)")
        db_conn.execute(a, customerid=newID, address=ads, email=emailAdd, creditcard=cCard, username=nick, password=psw, balance=bal)
        # Desconectamos la base de datos
        db_conn.close()
        return True
    except:
        if (db_conn is not None):
            db_conn.close()
        print("Exception in DB access:")
        print("-"*60)
        traceback.print_exc(file=sys.stderr)
        print("-"*60)
        return False

def db_getUserData(username):
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()

    mail = db_conn.execute("SELECT email FROM Customers WHERE username='"+str(username)+"'")
    ccard = db_conn.execute("SELECT creditcard FROM Customers WHERE username='"+str(username)+"'")
    address = db_conn.execute("SELECT address FROM Customers WHERE username='"+str(username)+"'")
    balance = db_conn.execute("SELECT balance FROM Customers WHERE username='"+str(username)+"'")
    # Desconectamos la base de datos
    db_conn.close()
    return list(mail, ccard, address, balance)

def db_getUserSaldo(username):
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()

    credit = db_conn.execute("SELECT balance FROM Customers WHERE username='"+str(username)+"'")
    # Desconectamos la base de datos
    db_conn.close()
    return credit

def db_updateUserSaldo(username, new):
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()

    db_conn.execute("UPDATE Customers SET Balance='"+str(new)+"' WHERE username='"+str(username)+"'")
    # Desconectamos la base de datos
    db_conn.close()
    return True

def db_userHistorial():
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()


    except:
        if (db_conn is not None):
            db_conn.close()
        print("Exception in DB access:")
        print("-"*60)
        traceback.print_exc(file=sys.stderr)
        print("-"*60)
        return False

"""--------- P E L I C U L A S ----------"""
def db_getMovie(id):
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()

    movie = db_conn.execute("SELECT * FROM imdb_movies WHERE movieid='"+str(id)+"'")
    
    # Desconectamos la base de datos
    db_conn.close()
    return movie


def db_getMovieDetail(id):
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()

    # inicializamos dict para guardar la info
    det = {}
    det['movieID'] = id
    
    #imdb_movies (TITULO + YEAR)
    tMovie = list(db_conn.execute("SELECT movietitle, year FROM imdb_movies WHERE imdb_movies.movieid = '"+str(id)+"'"))
    det['title'] = tMovie[0][0]
    det['year'] = tMovie[0][1]
    
    #imdb_moviegenres (GENRE) puede haber varios genres
    resGenres = list(db_conn.execute("SELECT m.genre FROM imdb_movies, imdb_moviegenres AS m WHERE imdb_movies.movieid = 1575 and m.movieid = '"+str(id)+"'"))
    movGenres = ""
    for g in resGenres:
        movGenres += str(g[0])+", "
    det['genres'] = movGenres[:-2]
    #imdb_movielanguages (LANGUAGE)   
    resLanguages = list(db_conn.execute("SELECT m.language FROM imdb_movies, imdb_movielanguages AS m WHERE imdb_movies.movieid = 1575 and m.movieid ='"+str(id)+"'"))
    movLanguages = ""
    for l in resLanguages:
        movLanguages += str(l[0])+", "
    det['languages'] = movLanguages[:-2]
    
    # imdb_moviecountries (COUNTRY)
    resCountries = list(db_conn.execute("SELECT m.country FROM imdb_movies, imdb_moviecountries AS m WHERE imdb_movies.movieid = 1575 AND m.movieid = '"+str(id)+"'"))
    movCountries = ""
    for c in resCountries:
        movCountries += str(c[0])+", "
    det['countries'] = movCountries[:-2]
    
    # imdb_directormovies (NOMBRE DIRECTOR)
    resDirector = list(db_conn.execute("SELECT d.directorid FROM imdb_movies, imdb_directormovies AS d WHERE imdb_movies.movieid = '"+str(id)+"'"))[0]
    resDirectorName = list(db_conn.execute("SELECT d.directorname FROM imdb_directors AS d WHERE d.directorid = 22108")) #problema a resolver
    det['director'] = resDirectorName[0][0]

    # products (PRECIO y DESC)
    tProduct = list(db_conn.execute("SELECT price, description FROM products WHERE movieid= '"+str(id)+"'"))[0]
    det['price'] = tProduct[0]
    det['description'] = tProduct[1]
    
    # actors
    tActors = db_conn.execute("SELECT a.actorname FROM imdb_actors AS a, imdb_actormovies AS m WHERE m.movieid = '"+str(id)+"' AND m.actorid=a.actorid")
    movActors = ""
    for a in tActors:
        movActors += str(a[0])+", "
    det['actors'] = movActors[:-2]
    
    # ratings
    tRatings = list(db_conn.execute("SELECT ratingmean, ratingcount FROM imdb_movies WHERE movieid= '"+str(id)+"'"))[0]
    det['ratingmean'] = tRatings[0]
    det['ratingcount'] = tRatings[1]

    db_conn.close()
    return det

def db_searchByName(name):
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()

    #movies = db_conn.execute("SELECT * FROM imdb_movies WHERE title LIKE '%"+str(name)+"%'")
    movies = db_conn.execute("SELECT * FROM imdb_movies WHERE movietitle LIKE '"+str(name)+"'")
    # Desconectamos la base de datos
    db_conn.close()
    return list(movies)

def db_searchByGenre(genre):
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()

    movies = db_conn.execute("SELECT * FROM imdb_movies, imdb_moviegenres WHERE imdb_moviegenres.genre = '"+str(genre)+"' AND imdb_movies.movieid = imdb_moviegenres.movieid  ORDER BY imdb_movies.movietitle LIMIT 10")
    # Desconectamos la base de datos
    db_conn.close()
    return list(movies)

def db_catalogo():
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()
    
    catalogo = db_conn.execute("SELECT * FROM imdb_movies WHERE imdb_movies.year=2006 OR imdb_movies.year=2005 ORDER BY imdb_movies.movietitle LIMIT 10")
    # Desconectamos la base de datos
    db_conn.close()
    return list(catalogo)

def db_getTopSales(year1, year2, l):
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()
    
    topSales = db_conn.execute("Select * FROM imdb_movies, getTopSales('"+str(year1)+"', '"+str(year2)+"') AS x WHERE imdb_movies.movieid = x.id LIMIT '"+str(l)+"'")
    # Desconectamos la base de datos
    db_conn.close()
    return list(topSales)

def db_getTopActors(genre):
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()

    a = text("Select * from getTopActors(:g)")
    topActors = db_conn.execute(a, g=genre)
    # Desconectamos la base de datos
    db_conn.close()
    return list(topActors)

def db_getGenres():
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()

    genres = db_conn.execute("SELECT * FROM genres")

    # Desconectamos la base de datos
    db_conn.close()
    return genres

def checkNull(cID, id):
    # conexion a la base de datos
    db_conn = None
    db_conn = db_engine.connect()

    aux = list(db_conn.execute("SELECT r.score FROM ratings AS r WHERE r.customerid='"+str(cID)+"' AND r.movieid='"+str(id)+"'"))
    if (len(aux)<=0):
        return True
    else:
        return False

def db_borraValoracion(id, nick):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        cID = db_getUserID(nick)
        db_conn.execute("DELETE FROM ratings WHERE customerid='"+str(cID)+"' AND movieid='"+str(id)+"'")

        # Desconectamos la base de datos
        db_conn.close()
        return True
    except:
        if (db_conn is not None):
            db_conn.close()
        print("Exception in DB access:")
        print("-"*60)
        traceback.print_exc(file=sys.stderr)
        print("-"*60)
        return False

def db_valoraPeli(voto, id, nick):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        cID = db_getUserID(nick)
        res = checkNull(cID, id)
        x=""
        if res==True:
            x=text("INSERT INTO ratings (customerid, movieid, score) VALUES (:cuID, :mID, :vot)")
        else:
            x=text("UPDATE ratings SET score=:vot WHERE movieid=:mID AND customerid=:cuID")
        
        db_conn.execute(x, vot=voto, cuID=cID, mID=id)
        # Desconectamos la base de datos
        db_conn.close()
        return True
    except:
        if (db_conn is not None):
            db_conn.close()
        print("Exception in DB access:")
        print("-"*60)
        traceback.print_exc(file=sys.stderr)
        print("-"*60)
        return False

"""--------- C O M P R A S ----------"""
def db_getCesta():
    return

def db_addToCarrito():
    return

def db_emptyCarrito():
    return

def db_payCarrito():
    return

