-- Una vez se disponga de esta información, realizar un procedimiento almacenado, 
-- setOrderAmount, que complete las columnas 'netamount' (suma de los precios de las 
-- películas del pedido) y 'totalamount' ('netamount' más impuestos) de la tabla 'orders' 
-- cuando éstas no contengan ningún valor. Invocad de forma manual la consulta de 
-- modificación setPrie y este procedimiento almacenado, para realizar una carga inicial

CREATE OR REPLACE FUNCTION setOrderAmount() returns void AS $$
BEGIN
    UPDATE orders
        SET netamount = aux.total,
            totalamount = aux.totalwt

        FROM (SELECT orders.orderid, sum(orderdetail.price*orderdetail.quantity) as total, sum(orderdetail.price*orderdetail.quantity) + orders.tax as totalwt  
            FROM orders, orderdetail
            WHERE orders.orderid = orderdetail.orderid
            GROUP BY orders.orderid) AS aux

        WHERE orders.orderid = aux.orderid;
END; $$
LANGUAGE 'plpgsql';

select setOrderAmount();

