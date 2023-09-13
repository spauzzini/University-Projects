--List the employees who report to those employees who report to the director.
--The director is the person who reports to no one. The output should show the
--“employeenumber” and the “lastname”


select e.employeenumber, e.lastname                                 -- Elements to be selected and printed

from employees e                                                    -- To select them from table employees

where e.reportsto in (select e2.employeenumber from employees e2    -- Nested queries that establish the conditions to
where e2.reportsto in (select e3.employeenumber from employees e3   -- to achieve the second level of "reportsto"
where e3.reportsto is NULL))
