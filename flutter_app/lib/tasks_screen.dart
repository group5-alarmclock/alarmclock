import 'package:app/pages/Alarm/days.dart';
import 'package:flutter/material.dart';
import 'package:hive_flutter/hive_flutter.dart';
import 'package:intl/intl.dart';
import 'package:numberpicker/numberpicker.dart';
import 'data/database.dart';
import 'util/dialog_box.dart';
import 'util/todo_tile.dart';
import 'package:table_calendar/table_calendar.dart';
import 'package:firebase_database/firebase_database.dart';

class TasksPage extends StatefulWidget {
  @override
  _TasksPageState createState() => _TasksPageState();
}

class _TasksPageState extends State<TasksPage> {
  DateTime selectedDay = DateTime.now();

  final myBox = Hive.box('mybox');
  TasksDataBase db = TasksDataBase();

  @override
  void initState() {
    if (myBox.get("TASKS") == null) {
      db.createInitialData();
    } else {
      db.loadData();
    }
    super.initState();
  }

  final dataBase = FirebaseDatabase.instance.reference();

  void onDaySelect(DateTime day, DateTime focusedDay) {
    print(day.toString());
    setState(() {
      for (var entry in db.tasksList.entries) {
        final key = entry.key;
        final value = entry.value;
        print('$key: $value');
      }

      selectedDay = day;
    });
  }

  final _controller = TextEditingController();

  void createNewTask(DateTime selectedDay) {
    showDialog(
      context: context,
      builder: (context) {
        TimeOfDay selectedTime = TimeOfDay.now();
        return StatefulBuilder(
          builder: (context, setState) {
            return AlertDialog(
              backgroundColor: Colors.grey[300],
              title: const Text('Create New Task'),
              content: Column(
                mainAxisSize: MainAxisSize.min,
                children: [
                  TextField(
                    controller: _controller,
                    decoration: const InputDecoration(labelText: 'Task'),
                  ),
                  const SizedBox(height: 12),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                    children: [
                      const SizedBox(width: 10),
                      NumberPicker(
                        minValue: 0,
                        maxValue: 23,
                        value: selectedTime.hour,
                        zeroPad: true,
                        infiniteLoop: true,
                        itemWidth: 80,
                        itemHeight: 60,
                        onChanged: (value) {
                          setState(() {
                            selectedTime = selectedTime.replacing(hour: value);
                          });
                        },
                        textStyle:
                            const TextStyle(color: Colors.grey, fontSize: 25),
                        selectedTextStyle:
                            const TextStyle(color: Color(0xFF008B8f), fontSize: 40),
                      ),
                      const Text(':',
                          style: TextStyle(color: Color(0xFF008B8f), fontSize: 45)),
                      NumberPicker(
                        minValue: 0,
                        maxValue: 59,
                        value: selectedTime.minute,
                        zeroPad: true,
                        infiniteLoop: true,
                        itemWidth: 80,
                        itemHeight: 60,
                        onChanged: (value) {
                          setState(() {
                            selectedTime =
                                selectedTime.replacing(minute: value);
                          });
                        },
                        textStyle:
                            const TextStyle(color: Colors.grey, fontSize: 25),
                        selectedTextStyle:
                            const TextStyle(color: Color(0xFF008B8f), fontSize: 40),
                      ),
                      // DayPage(
                      //     selectedTime: selectedTime,
                      //     selectedDay: "",
                      //     onDayChanged: handleDayChanged),
                    ],
                  ),
                ],
              ),
              actions: [
                TextButton(
                    onPressed: () {
                      Navigator.of(context).pop();
                    },
                    child: const Text('Cancel',
                        style: TextStyle(color: Color(0xFF008B8f)))),
                TextButton(
                  onPressed: () {
                    String hour = selectedTime.hour < 10
                        ? '0${selectedTime.hour}'
                        : selectedTime.hour.toString();
                    String minute = selectedTime.minute < 10
                        ? '0${selectedTime.minute}'
                        : selectedTime.minute.toString();
                    saveNewTask('$hour:$minute');
                  },
                  child: const Text('Save',style: TextStyle(color: Color(0xFF008B8f))),
                ),
              ],
            );
          },
        );
      },
    );
  }

  void saveNewTask(String time) {
    setState(() {
      var tasksList =
          db.tasksList[DateFormat('dd-MM-yyyy').format(selectedDay)];
      if (tasksList == null) {
        db.tasksList[DateFormat('dd-MM-yyyy').format(selectedDay)] = [
          [_controller.text, time]
        ];
      } else {
        tasksList.add([_controller.text, time]);
      }
    });
    db.updateDataBase();

    final child =
        dataBase.child('Tasks/${DateFormat('dd-MM-yyyy').format(selectedDay)}');
    child.update({time.toString(): _controller.text});

    _controller.clear();
    Navigator.of(context).pop();
  }

  void deleteTask(DateTime selectedDay, int index) {
    setState(() {
      final child = dataBase
          .child('Tasks/${DateFormat('dd-MM-yyyy').format(selectedDay)}');
      child.update({
        db.tasksList[DateFormat('dd-MM-yyyy').format(selectedDay)][index][1]:
            null
      });

      db.tasksList[DateFormat('dd-MM-yyyy').format(selectedDay)]
          ?.removeAt(index);
    });

    db.updateDataBase();
  }

  void handleDayChanged(String newDay) {
    setState(() {
      // selectedDay = newDay;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.grey[300],
        elevation: 0,
        title: const Text(
          'Tasks',
          style: TextStyle(color: Color(0xFF008B8f), fontSize: 22),
        ),
        actions: [
          IconButton(
            icon: const Icon(Icons.add, color: Color(0xFF008B8f), size: 33),
            onPressed: () => createNewTask(selectedDay),
          ),
        ],
      ),
      body: Column(
        children: [
          Container(
            alignment: Alignment.center,
            color: Colors.grey[300],
            child: TableCalendar(
              firstDay: DateTime.utc(2010, 10, 16),
              lastDay: DateTime.utc(2030, 3, 14),
              availableGestures: AvailableGestures.all,
              selectedDayPredicate: (day) => isSameDay(day, selectedDay),
              focusedDay: selectedDay,
              onDaySelected: onDaySelect,
              rowHeight: 53,
              daysOfWeekHeight: 30,
              calendarStyle: CalendarStyle(
                cellMargin: const EdgeInsets.all(2.0),
                todayDecoration: BoxDecoration(
                  color: Colors.grey[300],
                  shape: BoxShape.circle,
                ),
                todayTextStyle: const TextStyle(
                    color: Colors.red,
                    fontSize: 22,
                    fontWeight: FontWeight.w800),
                selectedDecoration: const BoxDecoration(
                  color: Color(0xFF008B8f),
                  shape: BoxShape.circle,
                ),
                defaultTextStyle: TextStyle(color: Colors.black, fontSize: 20),
                selectedTextStyle: TextStyle(color: Colors.black, fontSize: 20),
                weekendTextStyle: TextStyle(color: Colors.blue, fontSize: 20),
                outsideTextStyle: TextStyle(color: Colors.grey, fontSize: 20),
              ),
              headerStyle: const HeaderStyle(
                titleCentered: true,
                titleTextStyle: TextStyle(
                  color: Colors.black,
                  fontSize: 20,
                ),
                formatButtonVisible: false,
              ),
              weekendDays: const [5, 6],
              calendarBuilders: CalendarBuilders(
                defaultBuilder: (context, day, today) {
                  for (var entry in db.tasksList.entries) {
                    DateTime d = DateFormat('dd-MM-yyyy').parse(entry.key);
                    if (day.day == d.day &&
                        day.month == d.month &&
                        day.year == d.year) {
                      return Container(
                        child: Column(
                          children: [
                            Container(
                              width: 15,
                              height: 15,
                              decoration: BoxDecoration(
                                color: Colors.grey[300],
                                shape: BoxShape.circle,
                              ),
                            ),
                            Center(
                              child: Text(
                                '${day.day}',
                                style: const TextStyle(
                                    color: Colors.black, fontSize: 20),
                              ),
                            ),
                            Center(
                              child: Row(
                                mainAxisAlignment: MainAxisAlignment.center,
                                // Aligns the row in the center horizontally
                                children: List.generate(
                                  db.tasksList[DateFormat('dd-MM-yyyy')
                                              .format(d)
                                              ?.toString()] ==
                                          null
                                      ? 0
                                      : db
                                                  .tasksList[
                                                      DateFormat('dd-MM-yyyy')
                                                          .format(d)
                                                          ?.toString()]
                                                  ?.length <
                                              3
                                          ? db
                                              .tasksList[
                                                  DateFormat('dd-MM-yyyy')
                                                      .format(d)
                                                      ?.toString()]
                                              ?.length
                                          : 3,
                                  (index) => Container(
                                    width: 8,
                                    height: 8,
                                    decoration: const BoxDecoration(
                                      color: Colors.black,
                                      shape: BoxShape.circle,
                                    ),
                                  ),
                                ),
                              ),
                            ),
                          ],
                        ),
                      );
                    }
                  }
                  return null;
                },
                todayBuilder: (context, date, events) => Container(
                  // Add a dot under today's day
                  alignment: Alignment.bottomCenter,
                  margin: const EdgeInsets.only(bottom: 2),
                  decoration: BoxDecoration(
                    shape: BoxShape.circle,
                    color: date == selectedDay
                        ? Color(0xFF008B8f)
                        : Colors.grey[300],
                  ),
                  child: Container(
                    child: Column(
                      children: [
                        Container(
                          width: 14,
                          height: 14,
                          decoration: BoxDecoration(
                            color: date == selectedDay
                                ? Color(0xFF008B8f)
                                : Colors.grey[300],
                            shape: BoxShape.circle,
                          ),
                        ),
                        Center(
                          child: Text(
                            '${date.day}',
                            style: const TextStyle(
                                color: Colors.red,
                                fontSize: 22,
                                fontWeight: FontWeight.w600),
                          ),
                        ),
                        Center(
                          child: Row(
                            mainAxisAlignment: MainAxisAlignment.center,
                            // Aligns the row in the center horizontally
                            children: List.generate(
                              db.tasksList[DateFormat('dd-MM-yyyy')
                                          .format(date)
                                          ?.toString()] ==
                                      null
                                  ? 0
                                  : db
                                              .tasksList[
                                                  DateFormat('dd-MM-yyyy')
                                                      .format(date)
                                                      ?.toString()]
                                              ?.length <
                                          3
                                      ? db
                                          .tasksList[DateFormat('dd-MM-yyyy')
                                              .format(date)
                                              ?.toString()]
                                          ?.length
                                      : 3,
                              (index) => Container(
                                width: 8,
                                height: 8,
                                decoration: const BoxDecoration(
                                  color: Colors.black,
                                  shape: BoxShape.circle,
                                ),
                              ),
                            ),
                          ),
                        ),
                      ],
                    ),
                  ),
                ),
                  selectedBuilder: (context, date, today) => Container(
                    alignment: Alignment.bottomCenter,
                    margin: const EdgeInsets.only(bottom: 2),
                    decoration: BoxDecoration(
                      shape: BoxShape.circle,
                      color: date == selectedDay ? Color(0xFF008B8f) : Colors.grey[300],
                    ),
                    child: Container(
                      child: Column(
                        children: [
                          Container(
                            width: 14,
                            height: 14,
                            decoration: BoxDecoration(
                              color: date == selectedDay ? Color(0xFF008B8f) : Colors.grey[300],
                              shape: BoxShape.circle,
                            ),
                          ),
                          Center(
                            child: Text(
                              '${date.day}',
                              style: TextStyle(
                                color: date == selectedDay ? Colors.white : Colors.black,
                                fontSize: 20,
                              ),
                            ),
                          ),
                          Center(
                            child: Row(
                              mainAxisAlignment: MainAxisAlignment.center,
                              children: List.generate(
                                db.tasksList[DateFormat('dd-MM-yyyy')
                                    .format(date)
                                    ?.toString()] ==
                                    null
                                    ? 0
                                    : db.tasksList[
                                DateFormat('dd-MM-yyyy').format(date)?.toString()]
                                    ?.length,
                                    (index) => Container(
                                  width: 8,
                                  height: 8,
                                  decoration: const BoxDecoration(
                                    color: Colors.black,
                                    shape: BoxShape.circle,
                                  ),
                                ),
                              ),
                            ),
                          ),
                        ],
                      ),
                    ),
                  ),
              ),
            ),
          ),
          // const SizedBox(height: 16,)
          Expanded(
            child: Container(
              color: Colors.grey[300],
              child: ListView.builder(
                itemCount: db.tasksList[DateFormat('dd-MM-yyyy')
                            .format(selectedDay)
                            ?.toString()] ==
                        null
                    ? 0
                    : db
                        .tasksList[DateFormat('dd-MM-yyyy')
                            .format(selectedDay)
                            ?.toString()]
                        ?.length,
                itemBuilder: (context, index) {
                  List<dynamic>? tasks = db.tasksList[
                      DateFormat('dd-MM-yyyy').format(selectedDay)?.toString()];
                  tasks?.sort((a, b) =>
                      a[1].compareTo(b[1])); // Sort tasks based on time

                  String task = tasks?[index][0];
                  String time = tasks?[index][1];

                  List<Color> dotColors = [
                    Colors.blue,
                    Colors.green,
                    Colors.red,
                    Colors.orange,
                    Colors.pink,
                    Colors.yellow,
                    Colors.brown,
                  ];

                  Color dotColor = dotColors[index % dotColors.length];

                  return GestureDetector(
                    onTap: () {
                      showDialog(
                        context: context,
                        builder: (context) => AlertDialog(
                          title: Text('Task ${index + 1}'),
                          content: Text(task),
                          actions: [
                            TextButton(
                              onPressed: () {
                                Navigator.of(context).pop();
                              },
                              child: const Text(
                                'Close',
                                style: TextStyle(
                                  fontSize: 20,
                                ),
                              ),
                            ),
                          ],
                        ),
                      );
                    },
                    child: Container(
                      height: 80,
                      decoration: BoxDecoration(
                        color: Colors.white,
                        borderRadius: BorderRadius.circular(15),
                      ),
                      margin: EdgeInsets.symmetric(vertical: 6, horizontal: 10),
                      child: Center(
                        child: Row(
                          children: [
                            const Text('    '),
                            Container(
                              width: 14,
                              height: 14,
                              decoration: BoxDecoration(
                                color: dotColor,
                                shape: BoxShape.circle,
                              ),
                            ),
                            const SizedBox(width: 8),
                            Text(
                              '$time | ',
                              style: const TextStyle(
                                fontSize: 20,
                                fontWeight: FontWeight.w500,
                              ),
                            ),
                            Expanded(
                              flex: 2,
                              child: Text(
                                task,
                                style: const TextStyle(
                                  fontSize: 20,
                                  fontWeight: FontWeight.w500,
                                ),
                                overflow: TextOverflow.ellipsis,
                                maxLines: 1,
                              ),
                            ),
                            IconButton(
                              icon: Icon(
                                Icons.delete,
                                color: Colors.grey[900],
                                size: 28,
                              ),
                              onPressed: () => deleteTask(selectedDay, index),
                            ),
                          ],
                        ),
                      ),
                    ),
                  );
                },
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class TasksDataBase {
  Map<dynamic, dynamic> tasksList = {
    '20-6-2023': [
      ['seminar', '12:30'],
      ['exam', '16:30']
    ]
  };

  void createInitialData() {
    tasksList = {
      '20-06-2023': [
        ['seminar', '12:30'],
        ['exam', '16:30']
      ]
    };
  }

  final myBox = Hive.box('mybox');

  void loadData() {
    tasksList = myBox.get("TASKS");
  }

  void updateDataBase() {
    myBox.put("TASKS", tasksList);
  }
}
