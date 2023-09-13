# -*- coding: utf-8 -*-

import os
import sys, traceback, time
import sqlalchemy
import psycopg2
from pymongo import MongoClient

# configurar el motor de sqlalchemy
db_engine = sqlalchemy.create_engine("postgresql://alumnodb:alumnodb@localhost/si1", echo=False)

#conexión con la base de datos
try:
    db_conn = db_engine.connect()
    print("conectado")
except Exception:
    print("Error connecting database")


#datos búsqueda
nPelis = 400
pais = "UK"

#filtro géneros películas
q_movieGenres = "SELECT * FROM imdb_moviegenres WHERE movieid = %%ID%%;"
#filtro directores películas
q_movieDirectors = "SELECT DM.movieid, DR.directorname as directorname " \
                        "FROM imdb_directormovies as DM NATURAL JOIN imdb_directors as DR " \
                        "WHERE DM.movieid = %%ID%%;"
#filtro actores películas
q_movieActors = "SELECT AM.movieid, AR.actorname as actorname " \
                    "FROM imdb_actormovies as AM NATURAL JOIN imdb_actors as AR " \
                    "WHERE AM.movieid = %%ID%%;"
#filtro top películas
q_topMovies = "SELECT MV.movieid, MV.movietitle, MV.year, MC.country, MV.ratingcount, MV.ratingmean " \
                    "FROM imdb_movies AS MV, imdb_moviecountries AS MC " \
                    "WHERE MC.country= '%%PAIS%%' AND MV.movieid=MC.movieid "  \
                    "ORDER BY MV.year DESC " \
                    "LIMIT '%%nPelis%%';"
listaMovies = list(db_conn.execute(q_topMovies.replace("%%PAIS%%", pais).replace("%%nPelis%%", str(nPelis))))

#obtener películas
movies = []
for movie in listaMovies:
    if movie:
        #titulo
        title = movie["movietitle"].split("(")
        title = title[0]

        #año
        year = int(movie["year"])

        #géneros
        genres = []
        for genre in db_conn.execute(q_movieGenres.replace("%%ID%%", str(movie["movieid"]))):
            if genre and genre not in genres:
                genres.append(genre["genre"])

        #valoraciones recibidas
        number_of_votes = movie["ratingcount"]
        #valoracion media
        average_rating = movie["ratingmean"]

        #directores
        directors = []
        for director in db_conn.execute(q_movieDirectors.replace("%%ID%%", str(movie["movieid"]))):
            if director and director not in directors:
                directors.append(director["directorname"])

        #actores
        actors = []
        for actor in db_conn.execute(q_movieActors.replace("%%ID%%", str(movie["movieid"]))):
            if actor and actor not in actors:
                actors.append(actor["actorname"])

        #related movies
        relMovies = []
        for item in listaMovies:
            #es la misma peli q estamos analizando?
            if movie==item:
                #SI: saltamos
                continue
            #NO: calculamos coincidencias
            else:
                c=0
                for cat in genres:
                    coin = list(db_conn.execute("SELECT movieid FROM imdb_moviegenres WHERE genre='"+str(cat)+"' AND movieid='"+str(item[0])+"'"))
                    if len(coin)!=0:
                        c+=1
                avg = round(len(genres)/2)
                if c == avg:
                    if len(relMovies)!=10:
                        res = db_conn.execute("SELECT movietitle, year, ratingmean FROM imdb_movies WHERE movieid='"+str(item[0])+"'").fetchall()
                        relMovies.append({"title": res[0][0], "year": res[0][1], "average_rating": res[0][2]})
    
        #añadir película e info
        movies.append({"title": title,
                        "genres": genres,
                        "year": year,
                        "number_of_votes": number_of_votes,
                        "average_rating": average_rating,
                        "directors": directors,
                        "actors": actors,
                        "related_movies": relMovies})

#cerrar conexión con base
db_conn.close()

#conexión con mongo
client = MongoClient('localhost', 27017)
db = client["si1"]

#borra y añade topUk
if "topUk" not in db.list_collection_names():
    db.topUk.insert_many(movies)

client.close()