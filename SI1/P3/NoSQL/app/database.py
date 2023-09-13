# -*- coding: utf-8 -*-

import os
import sys, traceback, time

import sqlalchemy
from pymongo import MongoClient
from sqlalchemy import MetaData, Table, func
from sqlalchemy.sql.expression import *

# configurar el motor de sqlalchemy
db_engine = sqlalchemy.create_engine("postgresql://alumnodb:alumnodb@localhost/si1", echo=False, execution_options={"autocommit":False})

# Crea la conexión con MongoDB
mongo_client = MongoClient('localhost', 27017)
db = mongo_client["si1"]

def getMongoCollection(mongoDB_client):
    mongo_db = mongoDB_client.si1
    return mongo_db.topUK

def mongoDBCloseConnect(mongoDB_client):
    mongoDB_client.close()

def dbConnect():
    return db_engine.connect()

def dbCloseConnect(db_conn):
    db_conn.close()

def searchBy(title=None, year=None, genre=None, actor=None):
    #creamos la estructura donde se guardarán las expresiones de búsqueda
    search ={"$and":[]} #se usa $and para que se cumplan todas las condiciones

    #si se ha introducido un título, se añade a la búsqueda
    if title:
        search["$and"].append({"title": {"$regex": str(title), "$options": "i"}}) 
        
    #si se ha introducido un año, se añade a la búsqueda
    if year:
        if isinstance(year, list):
            search["$and"].append({"year": {"$in": year}})
        else:
            search["$and"].append({"year": {"$eq": year}})
    
    #si se ha introducido un género, se añade a la búsqueda
    if genre:
        if isinstance(genre, list):
            for g in genre:
                search["$and"].append({"genres": {"$regex": str(g), "$options": "i"}})
        else:
            search["$and"].append({"genres": {"$regex": str(genre), "$options": "i"}})
    
    #si se ha introducido un actor, se añade a la búsqueda
    if actor:
        if isinstance(actor, list):
            for a in actor:
                search["$and"].append({"actors": {"$regex": str(a), "$options": "i"}})
        else:
            search["$and"].append({"actors": {"$regex": str(actor), "$options": "i"}})
    
    movies = db.get_collection('topUk')
    return list(movies.find(search))
  
def delState(state, bFallo, bSQL, duerme, bCommit):
    
    # Array de trazas a mostrar en la página
    dbr=[]

    # TODO: Ejecutar consultas de borrado
    # - ordenar consultas según se desee provocar un error (bFallo True) o no
    # - ejecutar commit intermedio si bCommit es True
    # - usar sentencias SQL ('BEGIN', 'COMMIT', ...) si bSQL es True
    # - suspender la ejecución 'duerme' segundos en el punto adecuado para forzar deadlock
    # - ir guardando trazas mediante dbr.append()
    
    try:
        # Conexión bbdd
        db_conn = None
        db_conn = db_engine.connect()
        # TODO: ejecutar consultas

        if bSQL is True:
            db_conn.execute('BEGIN; ')

            # Comprobamos el numero de coincidencias a borrar antes y despues de ejecutar la consulta
            # para comprobar que ha funcionado
            query_nCustomers = "SELECT count(*) " \
                                "FROM customers NATURAL JOIN orders NATURAL JOIN orderdetail " \
                                "WHERE state='"+str(state)+"';"
            
            nCustomers = list(db_conn.execute(query_nCustomers))
            
            dbr.append("Entradas coincidentes de orderdetails antes de borrar:" + str(nCustomers[0][0]))

            del_orderdetail = "DELETE " \
                                "FROM orderdetail using customers NATURAL JOIN orders " \
                                "WHERE state='"+str(state)+"' AND orders.orderid=orderdetail.orderid;"
            
            db_conn.execute(del_orderdetail)
            nCustomers = list(db_conn.execute(query_nCustomers))
            
            dbr.append("Entradas coincidentes de orderdetails despues de borrar:" + str(nCustomers[0][0]))

            # Si el commit es True, creamos un punto de guardado para el rollback en caso de error

            if bCommit is True:
                dbr.append('Primer Commit')
                db_conn.execute('COMMIT; ')
                db_conn.execute('BEGIN; ')

            del_orders = "DELETE " \
                            "FROM orders using customers " \
                            "WHERE state='"+str(state)+"' AND customers.customerid=orders.customerid;"
                            
            del_customers = "DELETE " \
                            "FROM orders using customers " \
                            "WHERE state='"+str(state)+"';"

            # Si forzamos el fallo, alteramos el orden de ejecucion para producir un error
            if bFallo is True:

                dbr.append('Fallo forzado')
                
                query_totalC = "SELECT count(*) " \
                                "FROM customers " \
                                "WHERE state='"+str(state)+"';"
                                
                totalC = list(db_conn.execute(query_totalC))

                dbr.append("Entradas coincidentes de customers antes de borrar:" + str(totalC[0][0]))

                db_conn.execute(del_customers)
                totalC = list(db_conn.execute(query_totalC))

                dbr.append("Entradas coincidentes de customers despues de borrar:" + str(totalC[0][0]))


                query_totalC2 = "SELECT count(*) " \
                                "FROM customers NATURAL JOIN orders " \
                                "WHERE state='"+str(state)+"';"
                
                totalC2 = list(db_conn.execute(query_totalC2))

                dbr.append("Entradas coincidentes de orders antes de borrar:" + str(totalC2[0][0]))

                db_conn.execute(del_orders)
                totalC2 = list(db_conn.execute(query_totalC2))

                dbr.append("Entradas coincidentes de orders despues de borrar:" + str(totalC2[0][0]))

            # Si no hay fallo a producir, la ejecucion sigue el orden correcto
            else:
                dbr.append('Durmiendo ' + str(duerme) + ' segundo(s)')
                time.sleep(duerme)

                query_totalC2 = "SELECT count(*) " \
                                "FROM customers NATURAL JOIN orders " \
                                "WHERE state='"+str(state)+"';"
                
                totalC2 = list(db_conn.execute(query_totalC2))

                dbr.append("Entradas coincidentes de orders antes de borrar:" + str(totalC2[0][0]))

                db_conn.execute(del_orders)
                totalC2 = list(db_conn.execute(query_totalC2))

                dbr.append("Entradas coincidentes de orders despues de borrar:" + str(totalC2[0][0]))


                query_totalC = "SELECT count(*) " \
                                "FROM customers " \
                                "WHERE state='"+str(state)+"';"
                                
                totalC = list(db_conn.execute(query_totalC))

                dbr.append("Entradas coincidentes de customers antes de borrar:" + str(totalC[0][0]))

                db_conn.execute(del_customers)
                totalC = list(db_conn.execute(query_totalC))

                dbr.append("Entradas coincidentes de customers despues de borrar:" + str(totalC[0][0]))

        #Si bSQL es false, las consultas se realizan con SQLAlchemy

        else:
            transaccion = db_conn.begin()

            #Creacion de tablas auxiliares para las consultas de Alchemy
            db_meta = MetaData(bind=db_engine)

            customers = Table('customers', db_meta, autoload=True, autoload_with=db_engine)

            orders = Table('orders', db_meta, autoload=True, autoload_with=db_engine)

            orderdetail = Table('orderdetail', db_meta, autoload=True, autoload_with=db_engine)

            # Comprobamos el numero de coincidencias a borrar antes y despues de ejecutar la consulta para comprobar que ha funcionado
            query_nCustomers = select([func.count()]).select_from(customers.join(orders)).join(orderdetail).where(customers.c.state==state)
            
            nCustomers = list(db_conn.execute(query_nCustomers))
            
            dbr.append("Entradas coincidentes de orderdetails antes de borrar:" + str(nCustomers[0][0]))

            del_orderdetail = delete(orderdetail).where(customers.c.customerid == orders.c.customerid).where(orders.c.orderid == orderdetail.c.orderid).where(customers.c.state == state)
            
            db_conn.execute(del_orderdetail)

            nCustomers = list(db_conn.execute(query_nCustomers))
            
            dbr.append("Entradas coincidentes de orderdetails despues de borrar:" + str(nCustomers[0][0]))

            # Si bCommit es True creamos un punto de guardado para el rollback 
            
            if bCommit is True:
                dbr.append('Primer Commit')
                transaccion.commit()
                transaccion = db_conn.begin()

            del_orders = delete(orders).where(customers.c.customerid == orders.c.customerid).where(customers.c.state == state)

            del_customers = delete(customers).where(customers.c.state == state)

            # Si tenemos que forzar el fallo, alteramos el orden de ejecucion para producir un error
            if bFallo is True:
                dbr.append('Forzamos fallo')

                query_totalC = select([func.count()]).select_from(customers).where(customers.c.state == state)
                
                totalC = list(db_conn.execute(query_totalC))

                dbr.append("Entradas coincidentes de customers antes de borrar:" + str(query_totalC[0][0]))

                db_conn.execute(del_customers)
                totalC = list(db_conn.execute(query_totalC))

                dbr.append("Entradas coincidentes de customers despues de borrar:" + str(query_totalC[0][0]))

                query_totalC2 = select([func.count()]).select_from(customers.join(orders)).where(customers.c.state == state)
                
                totalC2 = list(db_conn.execute(query_totalC2))

                dbr.append("Entradas coincidentes de orders antes de borrar:" + str(totalC2[0][0]))

                db_conn.execute(del_orders)
                totalC2 = list(db_conn.execute(query_totalC2))

                dbr.append("Entradas coincidentes de orders despues de borrar:" + str(totalC2[0][0]))
            
            #Si no hay que producir un error, las consultas se ejecutan de manera natural
            else:

                dbr.append('Durmiendo ' + str(duerme) + ' segundo(s)')
                time.sleep(duerme)

                query_totalC2 = select([func.count()]).select_from(customers.join(orders)).where(customers.c.state == state)
                
                totalC2 = list(db_conn.execute(query_totalC2))

                dbr.append("Entradas coincidentes de orders antes de borrar:" + str(totalC2[0][0]))

                db_conn.execute(del_orders)
                totalC2 = list(db_conn.execute(query_totalC2))

                dbr.append("Entradas coincidentes de orders despues de borrar:" + str(totalC2[0][0]))

                query_totalC = select([func.count()]).select_from(customers).where(customers.c.state == state)
                
                totalC = list(db_conn.execute(query_totalC))

                dbr.append("Entradas coincidentes de customers antes de borrar:" + str(totalC[0][0]))

                db_conn.execute(del_customers)
                totalC = list(db_conn.execute(query_totalC))

                dbr.append("Entradas coincidentes de customers despues de borrar:" + str(totalC[0][0]))

    # TODO: deshacer en caso de error - rollback
    except Exception as e:
        dbr.append('Se ha producido un error (' + str(e) + ') en la transaccion')

        if bSQL is True:
            dbr.append('Rollback SQL')
            db_conn.execute('ROLLBACK; ')

            query_totalC = "SELECT count(*) " \
                                "FROM customers " \
                                "WHERE state='"+str(state)+"';"
                                
            totalC = list(db_conn.execute(query_totalC))

            dbr.append("Entradas coincidentes de customers despues rollback:" + str(totalC[0][0]))
            
            query_totalC2 = "SELECT count(*) " \
                                "FROM customers NATURAL JOIN orders " \
                                "WHERE state='"+str(state)+"';"
                
            totalC2 = list(db_conn.execute(query_totalC2))
            dbr.append("Entradas coincidentes de orders despues de rollback:" + str(totalC2[0][0]))

            query_nCustomers = "SELECT count(*) " \
                                "FROM customers NATURAL JOIN orders NATURAL JOIN orderdetail " \
                                "WHERE state='"+str(state)+"';"
            
            nCustomers = list(db_conn.execute(query_nCustomers))
            dbr.append("Entradas coincidentes de orders despues de rollback:" + str(nCustomers[0][0]))

        else:
            dbr.append('Rollback Transaccion')
            transaccion.rollback()

            query_totalC = select([func.count()]).select_from(customers).where(customers.c.state == state)

            totalC = list(db_conn.execute(query_totalC))
            dbr.append("Entradas coincidentes de customers despues rollback:" + str(totalC[0][0]))
            
            query_totalC2 = select([func.count()]).select_from(customers.join(orders)).where(customers.c.state == state)

            totalC2 = list(db_conn.execute(query_totalC2))
            dbr.append("Entradas coincidentes de orders despues de rollback:" + str(totalC2[0][0]))

            query_nCustomers = select([func.count()]).select_from(customers.join(orders)).join(orderdetail).where(customers.c.state==state)
            
            nCustomers = list(db_conn.execute(query_nCustomers))
            dbr.append("Entradas coincidentes de orders despues de rollback:" + str(nCustomers[0][0]))

    # TODO: confirmar cambios si todo va bien
    else:
        if bSQL is True:
            db_conn.execute('Commit;')
            dbr.append('Se ha completado la transaccion utilizando SQL')
        else:
            transaccion.commit()
            dbr.append('Se ha completado la transaccion utilizando SQLAlchemy')
        
        db_conn.close()
    return dbr

