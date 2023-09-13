
-- Sabiendo que los precios de las películas se han ido incrementando un 2% anualmente, 
-- elaborar la consulta setPrice.sql que complete la columna 'price' de la tabla 'orderdetail', 
-- sabiendo que el precio actual es el de la tabla 'products'.
UPDATE orderdetail
    SET price = aux.price
    FROM (SELECT products.price, orderdetail.orderid, orderdetail.prod_id 
            FROM products, orderdetail
             WHERE products.prod_id = orderdetail.prod_id) AS aux
    WHERE (orderdetail.orderid = aux.orderid AND orderdetail.prod_id = aux.prod_id);