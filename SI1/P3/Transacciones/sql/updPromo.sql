
--creacion de la columna promo con valor default 0
ALTER TABLE customers ADD COLUMN promo DECIMAL(6,2) DEFAULT 0;

DROP TRIGGER IF EXISTS tr_Promo ON customers;
CREATE OR REPLACE FUNCTION updPromo() RETURNS TRIGGER AS $$
BEGIN
    UPDATE orderdetail
    SET price = pr.price - (pr.price*(NEW.promo/100)) FROM products AS pr, orders AS od WHERE
    od.customerid = NEW.customerid AND od.status IS NULL AND od.orderid = orderdetail.orderid AND pr.prod_id = orderdetail.prod_id;
    PERFORM pg_sleep(10);
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER tr_Promo AFTER UPDATE ON customers FOR EACH ROW EXECUTE PROCEDURE updPromo();


UPDATE orders
SET status = null
WHERE orderid = 42135

UPDATE customers
SET promo = 50
WHERE customerid = 3199
