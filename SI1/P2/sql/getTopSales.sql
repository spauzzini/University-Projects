drop function if exists getTopSales(int,int);
CREATE OR REPLACE FUNCTION getTopSales(year1 INTEGER, year2 INTEGER) RETURNS TABLE(Year numeric, Film varchar, Sales numeric, Id INT) AS $$
BEGIN
    CREATE OR REPLACE VIEW filmsYear AS
        SELECT EXTRACT(year FROM orders.orderdate) as myYear, movies.movietitle as title, odt.quantity as qtt, movies.movieid AS myID
        FROM products, orders, orderdetail AS odt, imdb_movies AS movies
        WHERE products.prod_id = odt.prod_id AND products.movieid = movies.movieid AND odt.orderid = orders.orderid
    ;
    CREATE OR REPLACE VIEW sumYear AS
        SELECT myYear, title, sum(qtt) as freq, myID
        FROM filmsYear
        GROUP BY myYear, title, myID
    ;
    
    RETURN QUERY
	SELECT sumYear.myYear, sumYear.title, sumYear.freq, sumYear.myID
    FROM sumYear
    WHERE sumYear.myYear >= year1 AND sumYear.myYear <= year2
    ORDER BY sumYear.freq DESC;

END;
$$ LANGUAGE plpgsql;