CREATE TABLE USERS(
        id INTEGER  PRIMARY KEY AUTOINCREMENT,
        username TEXT NOT NULL UNIQUE,
        password TEXT NOT NULL

    );

CREATE TABLE FRIENDLIST (
    user1_id INTEGER NOT NULL,
    user2_id INTEGER NOT NULL,
    FOREIGN KEY (user1_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (user2_id) REFERENCES users(id) ON DELETE CASCADE,
    PRIMARY KEY (user1_id, user2_id)
);

    
DROP TABLE USERS;

INSERT INTO USERS(username, password)
VALUES ("Test", "testowy");

SELECT * FROM USERS;

