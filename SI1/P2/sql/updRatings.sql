DROP FUNCTION IF EXISTS updateRatings() CASCADE;
--funcion para actualizar la tabla ratings
CREATE FUNCTION updateRatings() RETURNS trigger AS $$
BEGIN
    --si se inserta una nueva valoración
    IF (TG_OP = 'INSERT') THEN
        --actualizamos la media de la película
        UPDATE imdb_movies
        SET ratingmean = (SELECT AVG(score) FROM ratings WHERE movieid = imdb_movies.movieid), ratingcount = (SELECT COUNT(score) FROM ratings WHERE movieid = imdb_movies.movieid)
        WHERE imdb_movies.movieid = NEW.movieid;
        RETURN NEW;
           
    --si se elimina una valoración
    ELSIF (TG_OP = 'DELETE') THEN
        --actualizamos la media de la película
        UPDATE imdb_movies
        SET ratingmean = (SELECT AVG(score) FROM ratings WHERE movieid = imdb_movies.movieid), ratingcount = (SELECT COUNT(score) FROM ratings WHERE movieid = imdb_movies.movieid)
        WHERE imdb_movies.movieid = OLD.movieid;
        RETURN OLD;
    --si se cambia la valoración
    ELSIF (TG_OP = 'UPDATE') THEN
        --actualizamos la media de la película
        UPDATE imdb_movies
        SET ratingmean = (SELECT AVG(score) FROM ratings WHERE movieid = imdb_movies.movieid)
        WHERE imdb_movies.movieid = NEW.movieid;
        RETURN NEW;
    END IF;
END; $$
LANGUAGE plpgsql;


CREATE trigger updRatings AFTER INSERT OR UPDATE OR DELETE ON ratings FOR EACH ROW EXECUTE FUNCTION updateRatings();