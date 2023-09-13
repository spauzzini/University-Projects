
--create index idx_country on customers(country); --descomentar para usar idx_country
--create index idx_orders on orders(date_part('year', orders.orderdate)); --descomentar para usar idx_orders


explain SELECT count(distinct(c.state)) FROM customers c NATURAL JOIN orders ord
    WHERE '2017' = date_part('year', ord.orderdate)
    AND 'Peru' = c.country;

--drop index idx_orders; --descomentar si se usa idx_orders
--drop index idx_country; --descomentar si se usa idx_country
    


