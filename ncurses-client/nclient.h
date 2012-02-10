struct chatmsg {
  char msg[1024];
  struct chatmsg *next;
  struct chatmsg *prev;
};

class ChatWindow {
  private:
    int _ypos;
    int _maxlines;
    int _messages;
    chatmsg* _lastmsg;

    void trim();
  public:
    ChatWindow( int, int);
    ~ChatWindow();
    bool addMessage( char*, char*);
    void draw();
};

ChatWindow::ChatWindow( int ypos, int vl ): 
  _ypos( ypos ),
  _maxlines( vl ),
  _lastmsg( NULL ),
  _messages( 0 ) {};

ChatWindow::~ChatWindow() {
  chatmsg *temp;
  while ( _lastmsg ) {
    temp = _lastmsg->next;
    free( temp );
  }
};

void ChatWindow::trim(){
  int num = 0;
  chatmsg *temp = _lastmsg,
          *del;
  
  while( temp->next ) {
    temp = temp->next;
  }
  del = temp;
  temp->prev->next = NULL;
  free( del );	
}

bool ChatWindow::addMessage( char *message, char* sender ) {
  if( !message || !sender )
    return false;

  chatmsg *temp = (chatmsg*)malloc(sizeof(chatmsg));

  strcat( temp->msg, sender );
  strcat( temp->msg, ": " );
  strcat( temp->msg, message);

  if ( _lastmsg ) {
    temp->next = _lastmsg;
    _lastmsg->prev = temp;
  } else {
    temp->next = NULL;
  }
  temp->prev = NULL;
  _lastmsg = temp;
  _messages++;
  if( _messages > _maxlines )
  	trim();
  return true;
};

void ChatWindow::draw() {
  int pos = LINES - 4;    
  move( _ypos, 1);
  hline( '-', COLS-2 );
 	move( LINES - 3, 1);
  hline( '-', COLS-2 );
  chatmsg *temp = _lastmsg;
  while( temp ) {
    mvaddstr( pos--, 2, temp->msg );
    temp = temp->next;
  }
};
