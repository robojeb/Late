struct _S_data;
struct _M_data;
struct _T_data;

struct _S_data {
  ~_S_data();
  enum type { S0, S1 } type_;

  union u {
    u():S0d_{nullptr, nullptr}{};
    ~u(){};
    std::tuple<_S_data*, _M_data*> S0d_;
    _M_data* S1d_;
  } data_;
};

struct _M_data {
  ~_M_data();
  enum type { M0, M1 } type_;

  union u{
    u():M0d_{nullptr, nullptr}{};
    ~u(){};
    std::tuple<_M_data*, _T_data*> M0d_;
    _T_data* M1d_;
  } data_;
};

struct _T_data {
  ~_T_data();
  enum type { T0, T1, T2, T3 } type_;
  union u{
    u():T0d_{""} {};
    ~u(){};
    std::string T0d_;
    std::string T1d_;
    std::string T2d_;
    std::string T3d_;
  } data_;
};


_S_data handle_S(_S_data input);

_M_data handle_M(_M_data input);

_T_data handle_M(_T_data input);
