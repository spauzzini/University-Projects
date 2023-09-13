--Reciba un parámetro “género” y que devuelva los actores o actrices
--que más veces han actuado en dicho género, ordenados de
--más actuaciones a menos, siempre que hayan trabajado en más de 4 películas de ese
--género, con información de la película en la que debutaron para ese género, el año de esa
--película y quién (o quiénes) dirigió esa película (pueden aparecer varios registros para el
--mismo actor porque hizo varias películas su primer año, varios directores para la misma
--película, etc.)

drop function if exists topActors(varchar) cascade;

CREATE OR REPLACE FUNCTION topActors(g varchar) returns table(Actor VARCHAR, Num BIGINT, Debut INT, Film VARCHAR, Director VARCHAR) AS $$
BEGIN
    --tabla con el número de películas de ese género que ha hecho cada actor (si es más de 4)
    create or replace view results AS
        select imdb_actors.actorname as actorname, count(*) as num
        from imdb_actors, imdb_actormovies, imdb_movies, imdb_moviegenres, imdb_directors, imdb_directormovies
        where imdb_actors.actorid = imdb_actormovies.actorid 
        and imdb_actormovies.movieid = imdb_movies.movieid
        and imdb_movies.movieid = imdb_moviegenres.movieid
        and imdb_moviegenres.genre = 'Action'
        and imdb_movies.movieid = imdb_directormovies.movieid
        and imdb_directormovies.directorid = imdb_directors.directorid
        group by imdb_actors.actorname
        having count(*)> 4
        order by count(*) desc
    ;
    
    return query
    select imdb_actors.actorname as actorname, results.num as num, imdb_movies.year as debut, imdb_movies.movietitle as movietitle, imdb_directors.directorname as directorname
    from imdb_actors, imdb_movies, imdb_moviegenres, imdb_actormovies, imdb_directormovies, imdb_directors, results
    where imdb_actors.actorid = imdb_actormovies.actorid
        and imdb_actormovies.movieid = imdb_movies.movieid
        and imdb_movies.movieid = imdb_moviegenres.movieid
        and imdb_movies.movieid = imdb_directormovies.movieid
        and imdb_directormovies.directorid = imdb_directors.directorid
        and results.actorname = imdb_actors.actorname
        and imdb_moviegenres.genre = 'Action'
    group by imdb_actors.actorname, imdb_movies.movietitle, imdb_directors.directorname, imdb_movies.year, results.num
    order by results.num desc;
END; 
$$ LANGUAGE plpgsql;

select * from topActors('Action');