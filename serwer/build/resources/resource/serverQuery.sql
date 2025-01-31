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

CREATE TABLE GROUPCHATS (
    chat_id INTEGER PRIMARY KEY AUTOINCREMENT,
    chat_name TEXT NOT NULL,
    owner_id INTEGER NOT NULL,
    FOREIGN KEY (owner_id) REFERENCES USERS(id)
);

CREATE TABLE USERS_IN_GROUPCHAT(
    chat_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    FOREIGN KEY (chat_id) REFERENCES GROUPCHATS(chat_id),
    FOREIGN KEY (user_id) REFERENCES USERS(id)
);

CREATE TABLE MESSAGES(
    message_id INTEGER PRIMARY KEY AUTOINCREMENT,
    chat_id INTEGER NOT NULL,
    sender_id INTEGER NOT NULL,
    content TEXT NOT NULL,
    FOREIGN KEY (chat_id) REFERENCES GROUPCHATS(chat_id),
    FOREIGN KEY (sender_id) REFERENCES USERS(id)
);



    





SELECT name FROM sqlite_master WHERE type='table';

INSERT INTO FRIENDLIST(user1_id, user2_id)
VALUES (1, 4),
        (4, 2);

INSERT INTO GROUPCHATS(chat_name, owner_id)
VALUES ('Testowy czat 1', 1),
       ('Czat 2', 4);

INSERT INTO USERS_IN_GROUPCHAT(chat_id, user_id)
VALUES (1, 5),
        (1, 3),
        (1, 4),
        (1,1);

INSERT INTO MESSAGES(chat_id, sender_id, content)
VALUES (1, 5, 'Wiadomosc 1'),
        (1, 5, 'Wiadomosc 2'),
        (1, 3, 'Wiadomosc 3'),
        (1, 4, 'To ja'),
        (1, 1, 'Dziendobry');

SELECT * FROM USERS;
SELECT * FROM GROUPCHATS;
SELECT * FROM USERS_IN_GROUPCHAT;
SELECT * FROM MESSAGES;
SELECT * FROM FRIENDLIST;


SELECT  g.chat_name, ug.chat_id
FROM GROUPCHATS g
JOIN USERS_IN_GROUPCHAT ug ON g.chat_id = ug.chat_id
JOIN USERS u ON ug.user_id = u.id
WHERE u.username = 'Mysiu';

SELECT m.MESSAGE_ID, u.USERNAME, m.CONTENT 
FROM MESSAGES m 
JOIN USERS u ON m.SENDER_ID = u.ID
WHERE m.CHAT_ID = 1;

SELECT g.chat_name, g.chat_id 
FROM GROUPCHATS g 
JOIN USERS_IN_GROUPCHAT ug on g.chat_id = ug.chat_id 
WHERE ug.user_id = 4;


DELETE FROM GROUPCHATS;
DELETE FROM USERS_IN_GROUPCHAT;
DELETE FROM MESSAGES;
DELETE FROM FRIENDLIST;

DELETE FROM sqlite_sequence WHERE name='MESSAGES';
DELETE FROM sqlite_sequence WHERE name='GROUPCHATS';
