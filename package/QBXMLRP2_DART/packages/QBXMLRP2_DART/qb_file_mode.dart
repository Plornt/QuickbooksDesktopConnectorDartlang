part of QBXMLRP2_DART;

class QBFileMode {
  static QBFileMode doNotCare = new QBFileMode._create(2);
  static QBFileMode multiUser = new QBFileMode._create(1);
  static QBFileMode singleUser = new QBFileMode._create(0);
  int _n;
  QBFileMode._create(int this._n);
}