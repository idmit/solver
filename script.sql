DROP DATABASE IF EXISTS `solver`;
CREATE DATABASE `solver`;
USE `solver`;

CREATE TABLE `Types`
(
id INT PRIMARY KEY AUTO_INCREMENT NOT NULL,
name VARCHAR(10) NOT NULL
);

CREATE TABLE `Methods`
(
id INT PRIMARY KEY AUTO_INCREMENT NOT NULL,
name VARCHAR(30) NOT NULL,
type_id INT NOT NULL,
FOREIGN KEY (type_id) REFERENCES Types (id)
    ON UPDATE CASCADE
    ON DELETE CASCADE
);

CREATE TABLE `Tasks`
(
id INT PRIMARY KEY AUTO_INCREMENT NOT NULL,
type_id INT NOT NULL,
FOREIGN KEY (type_id) REFERENCES Types (id)
    ON UPDATE CASCADE
    ON DELETE CASCADE
);

CREATE TABLE `Equations`
(
id INT PRIMARY KEY AUTO_INCREMENT NOT NULL,
task_id INT NOT NULL,
value DOUBLE NOT NULL,
left_right BOOL NOT NULL,
FOREIGN KEY (task_id) REFERENCES Tasks (id)
    ON UPDATE CASCADE
    ON DELETE CASCADE
);

CREATE TABLE `Solutions` 
(
id INT PRIMARY KEY AUTO_INCREMENT NOT NULL,
task_id INT NOT NULL,
method_id INT NOT NULL,
FOREIGN KEY (task_id) REFERENCES Tasks (id)
    ON UPDATE CASCADE
    ON DELETE CASCADE,
FOREIGN KEY (method_id) REFERENCES Methods (id)
    ON UPDATE CASCADE
    ON DELETE CASCADE
);

CREATE TABLE `Vals`
(
solution_id INT NOT NULL,
value DOUBLE NOT NULL,
FOREIGN KEY (solution_id) REFERENCES Solutions (id)
    ON UPDATE CASCADE
    ON DELETE CASCADE
);

CREATE TABLE `Meta` 
(
id INT PRIMARY KEY AUTO_INCREMENT NOT NULL,
solution_id INT,
task_id INT,
FOREIGN KEY (solution_id) REFERENCES Solutions (id)
    ON UPDATE CASCADE
    ON DELETE CASCADE,
FOREIGN KEY (task_id) REFERENCES Tasks (id)
    ON UPDATE CASCADE
    ON DELETE CASCADE,
name VARCHAR(50) NOT NULL,
value DOUBLE NOT NULL
);


INSERT INTO `Types` (name) VALUES ("LE"), ("SLAE"), ("ODE");

INSERT INTO `Methods` (name, type_id) VALUES ('Native', 1), ('Bisection', 1);
INSERT INTO `Methods` (name, type_id) VALUES ('Reflection', 2);
INSERT INTO `Methods` (name, type_id) VALUES ('Euler', 3);

INSERT INTO `Tasks` (type_id) VALUES (1);
INSERT INTO `Tasks` (type_id) VALUES (2);
INSERT INTO `Equations` (value, left_right, task_id) VALUES (10, 0, 1);
INSERT INTO `Equations` (value, left_right, task_id) VALUES (15, 1, 1);
INSERT INTO `Equations` (value, left_right, task_id) VALUES (3, 0, 2);
INSERT INTO `Equations` (value, left_right, task_id) VALUES (0, 0, 2);
INSERT INTO `Equations` (value, left_right, task_id) VALUES (30, 1, 2);
INSERT INTO `Equations` (value, left_right, task_id) VALUES (0, 0, 2);
INSERT INTO `Equations` (value, left_right, task_id) VALUES (7, 0, 2);
INSERT INTO `Equations` (value, left_right, task_id) VALUES (70, 1, 2);