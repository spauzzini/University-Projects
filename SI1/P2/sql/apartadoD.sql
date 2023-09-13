drop function if exists apartadoD(int,int);
CREATE OR REPLACE FUNCTION apartadoD(year1 INTEGER, year2 INTEGER) RETURNS TABLE(Year numeric, Film varchar, Sales numeric) AS $$
BEGIN
    CREATE OR REPLACE VIEW filmsYear1 AS
        SELECT EXTRACT(year FROM orders.orderdate) as myYear, movies.movietitle as title, odt.quantity as qtt
        FROM products, orders, orderdetail AS odt, imdb_movies AS movies
        WHERE products.prod_id = odt.prod_id AND products.movieid = movies.movieid AND odt.orderid = orders.orderid
    ;
    CREATE OR REPLACE VIEW sumYear1 AS
        SELECT myYear, title, sum(qtt) as freq
        FROM filmsYear1
        GROUP BY myYear, title
    ;
    CREATE OR REPLACE VIEW filmFreq1 AS
        SELECT myYear, max(freq) AS most
        FROM sumYear1
        GROUP BY myYear
    ;

    RETURN QUERY
	SELECT sumYear1.myYear, sumYear1.title, filmFreq1.most
    FROM filmFreq1, sumYear1
    WHERE filmFreq1.myYear = sumYear1.myYear AND filmFreq1.most = sumYear1.freq AND filmFreq1.myYear >= year1 AND filmFreq1.myYear <= year2
    ORDER BY filmFreq1.most DESC;

END;
$$ LANGUAGE plpgsql;

SELECT * FROM apartadoD(2015, 2018);