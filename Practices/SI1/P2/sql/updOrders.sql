drop function if exists updOrders() cascade;
--funcion para actualizar la tabla orders
create function updOrders() returns trigger as $$
begin
    --si se inserta un nuevo pedido
    if (TG_OP = 'INSERT') then
        --actualizamos el importe del pedido
        update orders
        --tenemos en cuenta el nuevo pedido(new.orderid) y recalculamos el importe
        --quantity ++ donde?
        set netamount = (select sum(price*quantity) from orderdetail where orderid = new.orderid),
            totalamount = (select sum(price*quantity) from orderdetail where orderid = new.orderid) + tax
        where orderid = new.orderid;
    --si se cambia un pedido
    elsif (TG_OP = 'UPDATE') then
        --actualizamos el importe del pedido
        update orders
        --intercambiamos el pedido antiguo (old.orderid) por el nuevo (new.orderid) y recalculamos el importe
        set netamount = (select sum(price*quantity) from orderdetail where orderid = new.orderid),
            totalamount = (select sum(price*quantity) from orderdetail where orderid = new.orderid) + tax
        where orderid = new.orderid;
    --si se elimina un pedido
    elsif (TG_OP = 'DELETE') then
        --actualizamos el importe del pedido
        update orders
        --borramos el pedido antiguo (old.orderid) y recalculamos el importe
        --quantity -- donde?
        set netamount = (select sum(price*quantity) from orderdetail where orderid = old.orderid),
            totalamount = (select sum(price*quantity) from orderdetail where orderid = old.orderid) + tax
        where orderid = old.orderid;
    end if;
    end if;
    return null;
end; $$
language plpgsql;

drop trigger updOrders on orderdetail;
create trigger updOrders after insert or update or delete on orderdetail for each row execute procedure updOrders();