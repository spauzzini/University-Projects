drop function if exists updInventoryAndCustomer() cascade;
--funcion para actualizar la tabla inventory y customer
create function updInventoryAndCustomer() returns trigger as $$
begin
    --si se inserta un nuevo elemento en el inventario
    if (TG_OP = 'INSERT') then
        --actualizamos el stock del producto
        update inventory
            --tenemos en cuenta el nuevo elemento(new.quantity) y recalculamos el stock
            set quantity = quantity + new.quantity
            where productid = new.productid;
    elsif (TG_OP = 'DELETE') then 
        --actualizamos el stock del producto
        update inventory
            --tenemos en cuenta el elemento eliminado(old.quantity) y recalculamos el stock
            set quantity = quantity - old.quantity
            where productid = old.productid;
        --actualizamos el saldo del cliente
        update customers
            set creditlimit = creditlimit - old.quantity*old.price
            where customerid = old.customerid;
    elsif (TG_OP = 'UPDATE') then
        --actualizamos el stock del producto
        update inventory
            set quantity = quantity - old.quantity + new.quantity
            where productid = new.productid;
        --actualizamos el credito del cliente
        update customers
            --tenemos en cuenta la nueva cantidad
            set creditlimit = creditlimit - old.quantity*old.price + new.quantity*new.price
            where customerid = new.customerid;
    end if;
end;
$$ language plpgsql;

drop trigger updInventoryAndCustomer on orders;
create trigger updInventoryAndCustomer after update on orders for each row execute procedure updInventoryAndCustomer();