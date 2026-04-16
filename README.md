бд
CREATE TABLE users(
    id SERIAL PRIMARY KEY,
    name varchar(100) NOT NULL,
    login VARCHAR(50) UNIQUE NOT NULL,
    hash TEXT NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL
);
