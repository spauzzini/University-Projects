#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from app import app
from app import database
from flask import render_template, request, url_for
import os
import sys
import time

@app.route('/', methods=['POST','GET'])
@app.route('/index', methods=['POST','GET'])
def index():
    return render_template('index.html')


@app.route('/borraEstado', methods=['POST','GET'])
def borraEstado():
    if 'state' in request.form:
        state    = request.form["state"]
        bSQL    = request.form["txnSQL"]
        bCommit = "bCommit" in request.form
        bFallo  = "bFallo"  in request.form
        duerme  = request.form["duerme"]
        dbr = database.delState(state, bFallo, bSQL=='1', int(duerme), bCommit)
        return render_template('borraEstado.html', dbr=dbr)
    else:
        return render_template('borraEstado.html')

    
@app.route('/topUK', methods=['POST','GET'])
def topUK():

    # Una tabla con toda la información de aquellas películas (documentos) que sean comedias comprendidas entre 1990 y 1992.
    m1 = database.searchBy(genre="Comedy", year=list(range(1990, 1992+1)))
  
    # Una tabla con toda la información de aquellas películas (documentos) de acción de los años 1995, 1997 y 1998, y que empiecen por la palabra “The” (por ejemplo "Shooter, The").
    m2 = database.searchBy(title=", The", genre="Action", year=[1995,1997,1998])

    # Una tabla con toda la información de aquellas películas (documentos) en las que Darren McAree y Katie Lockett hayan compartido reparto.
    m3 = database.searchBy(actor=["McAree, Darren", "Lockett, Katie"])

    movies=[[m1],[m2],[m3]]
    return render_template('topUK.html', movies=movies)