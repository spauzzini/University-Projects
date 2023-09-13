------------RELACIONES ENTRE TABLAS--------
--hacemos que numparticipation en imdb_dicrectormovies no sea primary
ALTER TABLE imdb_directormovies
  DROP CONSTRAINT imdb_directormovies_pkey;

ALTER TABLE imdb_directormovies
    ADD CONSTRAINT imdb_directormovies_pkey PRIMARY KEY (directorid, movieid);

--hacemos que extrainformation en imdb_movielanguages no sea primary
ALTER TABLE imdb_movielanguages
    DROP CONSTRAINT imdb_movielanguages_pkey;

ALTER TABLE imdb_movielanguages
	ADD CONSTRAINT imdb_movielanguages_pkey PRIMARY KEY (movieid, "language");

--hacemos que actorid y movieid sean primary keys en imdb_actormovies
ALTER TABLE imdb_actormovies
    ADD CONSTRAINT imdb_movieactormovies_pkey PRIMARY KEY (actorid, movieid);

--relacion entre imdb_movies y imdb_actormovies, queremos que movieid sea foreign key
ALTER TABLE imdb_actormovies
    ADD CONSTRAINT imdb_actormovies_movieid_fkey FOREIGN KEY (movieid) REFERENCES imdb_movies(movieid) ON UPDATE CASCADE ON DELETE CASCADE;
    
--relacion entre imdb_actors y imdb_actormovies, queremos que actorid sea foreign key
ALTER TABLE imdb_actormovies
    ADD CONSTRAINT imdb_actormovies_actorid_fkey FOREIGN KEY (actorid) REFERENCES imdb_actors(actorid) ON UPDATE CASCADE ON DELETE CASCADE;
    
--hacemos que prod_id en inventory sea foreign key
ALTER TABLE inventory
    ADD CONSTRAINT imdb_inventory_prod_id_fkey FOREIGN KEY (prod_id) REFERENCES products(prod_id);

--creamos neworderdetail auxiliar donde juntamos orderid, prodid, price y quantity (para evitar que se repita orderid y prodid)
CREATE TABLE neworderdetail as (select orderid, prod_id, sum(price) as price, sum(quantity) as quantity from orderdetail GROUP BY orderid, prod_id);
--borramos orderdetail anterior
DROP TABLE orderdetail;
--hacemos que nuestra auxiliar sea la principal renombrándola
ALTER TABLE neworderdetail rename to orderdetail;

--hacemos que orderid y prod_id en orderdetail sean primary key
ALTER TABLE orderdetail
    ADD CONSTRAINT orderdetail_pkey PRIMARY KEY (prod_id, orderid);

--hacemos que orderid en orderdetail sea foreign key
ALTER TABLE orderdetail
    ADD CONSTRAINT orderdetail_orderid_fkey FOREIGN KEY (orderid) REFERENCES orders(orderid) ON DELETE CASCADE;

--hacemos que orderid en prod_id sea foreign key
ALTER TABLE orderdetail
    ADD CONSTRAINT orderdetail_prod_id_fkey FOREIGN KEY (prod_id) REFERENCES products(prod_id) ON DELETE CASCADE;

--hacemos que customerid en orders sea foreign key
ALTER TABLE orders
    ADD CONSTRAINT orders_customerid_fkey FOREIGN KEY (customerid) REFERENCES customers(customerid) ON UPDATE CASCADE ON DELETE CASCADE;

------------FORMATOS--------
--cambiamos formato year de imdb_movies de texto a entero de 4 dígitos
ALTER TABLE imdb_movies ALTER COLUMN year TYPE integer USING (LEFT(year, 4)::integer);

--creamos enumeracion status orders
CREATE TYPE enum_status as ENUM ('Paid', 'Processed', 'Shipped');
--asignamos esta enumeración a la columna status de orders
ALTER TABLE orders ALTER COLUMN status TYPE enum_status USING status::enum_status;

--cambiamos formato contraseña 97 caracteres
ALTER TABLE customers ALTER COLUMN password TYPE text USING (LEFT(password, 97)::text);



------------AÑADIR--------
--campo balance a tabla customers
ALTER TABLE customers
    ADD COLUMN balance decimal;
--crear tabla ratings
CREATE TABLE ratings as (select c.customerid, m.movieid from customers c, imdb_movies m GROUP BY c.customerid, m.movieid);
ALTER TABLE ratings 
    ADD COLUMN score integer;

--hacer customerid y movieid de ratings primary key
ALTER TABLE ratings
    ADD CONSTRAINT ratings_pkey PRIMARY KEY (customerid, movieid);

--hacemos movieid de ratings foreign key
ALTER TABLE ratings
    ADD CONSTRAINT ratings_movieid_fkey FOREIGN KEY (movieid) REFERENCES imdb_movies(movieid) ON UPDATE CASCADE ON DELETE CASCADE;

--hacemos customerid de ratings foreign key
ALTER TABLE ratings
    ADD CONSTRAINT ratings_customerid_fkey FOREIGN KEY (customerid) REFERENCES customers(customerid) ON UPDATE CASCADE ON DELETE CASCADE;
    
--ratingmean y ratingcount a imdb_movies
ALTER TABLE imdb_movies
    ADD COLUMN ratingmean decimal;

ALTER TABLE imdb_movies
    ADD COLUMN ratingcount integer;

--creamos setCustomersBalance
CREATE OR REPLACE FUNCTION setCustomersBalance(IN initialBalance bigint) returns void AS $$
BEGIN
    UPDATE customers
        SET balance = ROUND(cast(((initialBalance - 0) * random()+0) as numeric), 2);
END; $$
LANGUAGE 'plpgsql';

--llamamos a la funcion
SELECT setCustomersBalance(100);


--Para garantizar la integridad de los datos (los valores posibles de las columnas), crear las
--tablas correspondientes y convertir los atributos multivaluados ‘moviecountries’,
--‘moviegenres’ y ‘movielanguages’ en relaciones entre la tabla ‘movies’ y las tablas creadas.
--Estos cambios también se incorporarán al script actualiza.sql.

--hay que borrar imdb_movielanguages, imdb_moviegenres y imdb_moviecountries y relaciones?? OJO por las queries donde se usaba lo de antes
CREATE TABLE languages AS (SELECT DISTINCT language FROM imdb_movielanguages);
ALTER TABLE languages ADD PRIMARY KEY (language);
ALTER TABLE imdb_movielanguages ADD FOREIGN KEY (language) REFERENCES languages(language);

CREATE TABLE genres AS (SELECT DISTINCT genre from imdb_moviegenres);
ALTER TABLE genres ADD PRIMARY KEY (genre);
ALTER TABLE imdb_moviegenres ADD FOREIGN KEY (genre) REFERENCES genres(genre);

CREATE TABLE countries AS (SELECT DISTINCT country from imdb_moviecountries);
ALTER TABLE countries ADD PRIMARY KEY (country);
ALTER TABLE imdb_moviecountries ADD FOREIGN KEY (country) REFERENCES countries(country);

ALTER TABLE imdb_movies ADD COLUMN language text;
ALTER TABLE imdb_movies ADD FOREIGN KEY (language) REFERENCES languages(language);
ALTER TABLE imdb_movies ADD COLUMN genre text;
ALTER TABLE imdb_movies ADD FOREIGN KEY (genre) REFERENCES genres(genre);
ALTER TABLE imdb_movies ADD COLUMN country text;
ALTER TABLE imdb_movies ADD FOREIGN KEY (country) REFERENCES countries(country);